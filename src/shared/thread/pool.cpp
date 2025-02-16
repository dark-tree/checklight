
#include "pool.hpp"

/*
 * TaskPool
 */

void TaskPool::run() {

	ManagedTask task;
	while (true) {
		{
			std::unique_lock<std::mutex> lock(queue_mutex);

			// wait for task to appear in tasks queue or for the stop sequence to begin
			condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
			if (stop && tasks.empty()) return;

			task = std::move(tasks.front());
			tasks.pop();
		}

		task.call();
	}

}

size_t TaskPool::optimal() {
	return std::max((int) std::thread::hardware_concurrency() - 1, 1);
}

TaskPool::TaskPool(size_t count)
: stop(false) {
	std::cout << "Created thread pool with " << count << " workers\n";

	while (count --> 0) {
		workers.emplace_back(&TaskPool::run, this);
	}
}

TaskPool::~TaskPool() {
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}

	this->condition.notify_all();

	for (auto& worker : this->workers) {
		worker.join();
	}
}

void TaskPool::enqueue(const Task& task) {
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		// don't allow enqueueing after stopping the pool
		if (stop) {
			throw std::runtime_error("Unable to add task to a stopped pool!");
		}

		tasks.emplace(task);
	}

	condition.notify_one();
}