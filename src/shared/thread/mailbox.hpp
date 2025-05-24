#pragma once

#include "external.hpp"
#include "pool.hpp"

/**
 * A class that describes a "mailbox" strategy for adding tasks
 * to the specified during construction task pool
 */
class MailboxTaskDelegator {

	private:

		std::mutex mutex;
		TaskPool& pool;

	public:

		MailboxTaskDelegator(TaskPool& pool);

		/**
		 * Enqueue task to the parent pool in such a way that
		 * only one task will be allowed to execute at a time
		 * if enqueue is called before the previous task is done
		 * it will block until the previous task completes
		 */
		void enqueue(const Task& task);

		/**
		 * Wait for the previously added task to complete,
		 * can be safely called even if no task has yet been enqueue
		 */
		void wait();

};