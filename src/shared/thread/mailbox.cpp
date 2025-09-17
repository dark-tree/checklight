
#include "mailbox.hpp"

MailboxTaskDelegator::MailboxTaskDelegator(TaskPool& pool)
: pool(pool) {}

void MailboxTaskDelegator::enqueue(const Task& task) {
	mutex.lock();

	pool.chained(task, [this] () {
		mutex.unlock();
	});
}

void MailboxTaskDelegator::wait() {
	std::lock_guard lock{mutex};
}