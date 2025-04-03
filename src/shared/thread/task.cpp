
#include "task.hpp"

/*
 * ManagedTask
 */

ManagedTask::ManagedTask(const Task& task)
: task(task), timer() {}

void ManagedTask::call() const {
	double millis = timer.milliseconds();

	if (millis > 200) {
		printf("WARN: Is the system overloaded? Task waited %dms before starting execution!", (int) millis);
	}

	task();
}