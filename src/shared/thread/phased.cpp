
#include "phased.hpp"

PhasedTaskDelegator::PhasedTaskDelegator(TaskPool& pool)
: pool(pool) {}

void PhasedTaskDelegator::enqueue(const Task& task) {
	{
		std::unique_lock lock(mutex);
		working ++;
	}

	// first execute task on the thread pool, after it completes run out lambda
	pool.chained(task, [this] () {
		std::unique_lock lock(mutex);

		if (--working <= 0) {
			lock.unlock();
			condition.notify_one();
		}
	});
}

void PhasedTaskDelegator::wait() {
	std::unique_lock lock(mutex);

	while (working > 0) {
		condition.wait(lock);
	}
}