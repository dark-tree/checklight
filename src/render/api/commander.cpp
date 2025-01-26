
#include "commander.hpp"
#include "render/system.hpp"

/*
 * RenderCommander
 */

RenderCommander::RenderCommander(Type type, CommandBuffer buffer, CommandRecorder recorder, TaskQueue queue)
: type(type), buffer(buffer), recorder(recorder), queue(queue) {};

CommandRecorder& RenderCommander::getRecorder() {
	return recorder;
}

TaskQueue& RenderCommander::getTaskQueue() {
	return queue;
}

void RenderCommander::complete() {
	RenderSystem& system = *RenderSystem::system;

	if (type == Type::TRANSIENT) {
		recorder.done();

		// submit and wait
		Fence fence = system.createFence();
		buffer.submit().signal(fence).done(system.queue);
		fence.wait();

		// cleanup after ourselves
		fence.close();
		buffer.close();

		// run client cleanups tasks
		queue.execute();
	}

	if (type == Type::ASYNCHRONOUS) {
		system.getFrame().queue.addAll(queue);
	}
}