#pragma once

#include <utility>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <future>

#include "task.hpp"

/**
 * A basic thread pool implementation with
 * support for std::futures
 */
class TaskPool {

	private:

		bool stop;

		std::vector<std::thread> workers;
		std::mutex queue_mutex;
		std::queue<ManagedTask> tasks;
		std::condition_variable condition;

		void run();

	public:

		TaskPool(size_t count = TaskPool::optimal());
		~TaskPool();

		/**
		 * Get the "optimal" number of threads for a
		 * thread pool on this hardware
		 */
		static size_t optimal();

		/**
		 * Enqueue a task for execution by one of the threads
		 * on this thread pool, the tasks start execution in a FIFO order
		 */
		void enqueue(const Task& task);

	public:

		template <typename Func, typename Arg, typename... Args>
		void enqueue(Func func, Arg arg, Args... args) {
			this->enqueue(std::bind(func, arg, args...));
		}

		/**
		 * Executes two tasks after one after another, ensures the second
		 * task runs even if an error occurs during the first one
		 *
		 * @param first the first task to execute
		 * @param then the seconds task to execute on the same thread
		 */
		template <typename First, typename Then>
		void chained(const First& first, const Then& then) {
			enqueue([first, then] () {
				try {
					first();
				} catch (std::runtime_error& exception) {
					printf("WARN: Exception in primary stage of chained task: %s\n", exception.what());
				} catch (...) {
					printf("WARN: Unknown error in primary stage of chained task!\n");
				}

				then();
			});
		}

		/**
		 * Wraps the given function in a std::future and returns it
		 * while enqueuing the task for execution on this thread pool
		 */
		template <typename F, typename T = typename std::invoke_result<F>::type>
		std::future<T> defer(const F& task) {
			auto* raw_promise = new std::promise<T>();
			std::future<T> future = raw_promise->get_future();

			// kill me
			enqueue([task, raw_promise] () {
				std::unique_ptr<std::promise<T>> promise(raw_promise);

				try {
					promise->set_value(task());
				} catch (...) {
					promise->set_exception(std::current_exception());
				}
			});

			return future;
		}

};