
#include "queue.hpp"

/*
 * TaskQueue
 */

TaskQueue::TaskQueue(const TaskQueue& other)
: tasks(other.tasks) {}

TaskQueue::TaskQueue(TaskQueue&& other) noexcept
: tasks(std::move(other.tasks)) {}

void TaskQueue::enqueue(const std::function<void()>& task) {
	std::lock_guard lock(queue_mutex);
	tasks.emplace(task);
}

std::vector<std::function<void()>> TaskQueue::toVector() {
	std::vector<std::function<void()>> locals;
	locals.reserve(tasks.size());

	{
		std::lock_guard lock(queue_mutex);

		while (!tasks.empty()) {
			locals.emplace_back(std::move(tasks.front()));
			tasks.pop();
		}
	}

	return locals;
}

void TaskQueue::addAll(TaskQueue& queue) {
	auto locals = queue.toVector();

	std::lock_guard lock(queue_mutex);
	for (const auto& task : locals) {
		enqueue(task);
	}
}

int TaskQueue::execute() {
	auto locals = toVector();
	int count = locals.size();

	for (std::function<void()>& task : locals) {
		task();
	}

	return count;
}