
#include "frame.hpp"
#include "renderer.hpp"

/*
 * RenderFrame
 */

RenderFrame::RenderFrame(Renderer& renderer, const CommandPool& pool, const LogicalDevice& device)
: buffer(pool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY)), available_semaphore(device.getHandle()), finished_semaphore(device.getHandle()), flight_fence(device.getHandle(), true) {
	uniform_buffer = renderer.allocator.allocateBuffer(Memory::SHARED, sizeof(SceneUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, "Frame Uniform");

	set_immediate = renderer.descriptor_pool.allocate(renderer.layout_immediate);

	set_compose = renderer.descriptor_pool.allocate(renderer.layout_compose);
	set_compose.sampler(0, renderer.attachment_albedo.getTexture(), VK_IMAGE_LAYOUT_GENERAL);

	// intentionally don't set TLAS, it's not build yet at this point!
	set_raytrace = renderer.descriptor_pool.allocate(renderer.layout_raytrace);
	set_raytrace.view(1, renderer.attachment_albedo.getView(), VK_IMAGE_LAYOUT_GENERAL);
	set_raytrace.buffer(2, uniform_buffer, sizeof(SceneUniform));
}

RenderFrame::~RenderFrame() {
	uniform_buffer.close();
	buffer.close();
	available_semaphore.close();
	finished_semaphore.close();
	flight_fence.close();
}

void RenderFrame::flushUniformBuffer(CommandRecorder& recorder) {
	recorder.updateBuffer(uniform_buffer, &uniforms);
}

void RenderFrame::wait() {
	flight_fence.sync();
}

void RenderFrame::execute() {
	queue.execute();
}