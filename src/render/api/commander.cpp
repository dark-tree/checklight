
#include "commander.hpp"
#include "render/system.hpp"

/*
 * RenderCommander
 */

RenderCommander::RenderCommander(CommandBuffer buffer, CommandRecorder recorder, TaskQueue queue)
: buffer(buffer), recorder(recorder), queue(queue) {};

CommandRecorder& RenderCommander::getRecorder() {
	return recorder;
}

TaskQueue& RenderCommander::getTaskQueue() {
	return queue;
}

void RenderCommander::complete() {
	RenderSystem& system = *RenderSystem::system;

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