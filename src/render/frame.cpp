
#include "frame.hpp"
#include "renderer.hpp"

/*
 * RenderFrame
 */

RenderFrame::RenderFrame(Renderer& renderer, const CommandPool& pool, const LogicalDevice& device)
: buffer(pool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY)), available_semaphore(device.getHandle()), finished_semaphore(device.getHandle()), flight_fence(device.getHandle(), true) {
	set_0 = renderer.descriptor_pool.allocate(renderer.layout_geometry);
}

RenderFrame::~RenderFrame() {
	buffer.close();
	available_semaphore.close();
	finished_semaphore.close();
	flight_fence.close();
}

void RenderFrame::wait() {
	flight_fence.sync();
}

void RenderFrame::execute() {
	queue.execute();
}