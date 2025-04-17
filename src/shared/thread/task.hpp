#pragma once

#include "external.hpp"
#include "shared/timer.hpp"

using Task = std::function<void()>;

/**
 * A wrapper around a std::function that allows
 * TaskPool to attach additional information to tasks
 */
class ManagedTask {

	private:

		Task task;
		Timer timer;

	public:

		ManagedTask() = default;
		ManagedTask(const Task& task);

		void call() const;

};