#pragma once

#include "pool.hpp"

/**
 * A class that describes a barrier-like strategy for adding tasks
 * to the specified during construction task pool
 */
class PhasedTaskDelegator {

	private:

		int working = 0;
		std::mutex mutex;
		std::condition_variable condition;
		TaskPool& pool;

	public:

		PhasedTaskDelegator(TaskPool& pool);

		/**
		 * Enqueue task to the parent pool, all enqueue tasks will
		 * need to finish before the wait() method will be unlocked.
		 */
		void enqueue(const Task& task);

		/**
		 * Wait for all the previously added tasks to complete,
		 * can be safely called even if no task has yet been enqueue
		 */
		void wait();

};