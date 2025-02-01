#pragma once

#include "external.hpp"

class TaskQueue {

	private:

		std::mutex queue_mutex;
		std::queue<std::function<void()>> tasks;

	public:

		TaskQueue() = default;
		TaskQueue(const TaskQueue& other);
		TaskQueue(TaskQueue&& other) noexcept;

		void enqueue(const std::function<void()>& task);

		template <typename Func, typename Arg, typename... Args>
		void enqueue(Func func, Arg arg, Args... args) {
			enqueue(std::bind(func, arg, args...));
		}

		std::vector<std::function<void()>> toVector();
		void addAll(TaskQueue& queue);

	public:

		/**
		 * Execute all pending task in this queue
		 */
		int execute();

};

