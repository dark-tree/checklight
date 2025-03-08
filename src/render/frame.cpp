
#include "frame.hpp"
#include "renderer.hpp"

/*
 * RenderFrame
 */

RenderFrame::RenderFrame(Renderer& renderer, const CommandPool& pool, const LogicalDevice& device)
: buffer(pool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY)), available_semaphore(device.getHandle()), finished_semaphore(device.getHandle()), flight_fence(device.getHandle(), true) {
	uniform_buffer = renderer.allocator.allocateBuffer(Memory::SHARED, sizeof(SceneUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, "Frame Uniform");

	set_immediate = renderer.descriptor_pool.allocate(renderer.layout_immediate);
	set_immediate.buffer(1, uniform_buffer, sizeof(SceneUniform));

	set_compose = renderer.descriptor_pool.allocate(renderer.layout_compose);
	set_compose.sampler(0, renderer.attachment_albedo.getTexture(), VK_IMAGE_LAYOUT_GENERAL);
	set_compose.sampler(1, renderer.attachment_illumination.getTexture(), VK_IMAGE_LAYOUT_GENERAL);
	set_compose.sampler(2, renderer.attachment_normal.getTexture(), VK_IMAGE_LAYOUT_GENERAL);
	set_compose.sampler(3, renderer.attachment_soild_illumination.getTexture(), VK_IMAGE_LAYOUT_GENERAL);

	set_denoise2 = renderer.descriptor_pool.allocate(renderer.layout_denoise2);
	set_denoise2.sampler(0, renderer.attachment_illum_transport.getTexture(), VK_IMAGE_LAYOUT_GENERAL);
	set_denoise2.sampler(1, renderer.attachment_normal.getTexture(), VK_IMAGE_LAYOUT_GENERAL);

	set_denoise = renderer.descriptor_pool.allocate(renderer.layout_denoise);
	set_denoise.sampler(0, renderer.attachment_illumination.getTexture(), VK_IMAGE_LAYOUT_GENERAL);
	set_denoise.sampler(1, renderer.attachment_normal.getTexture(), VK_IMAGE_LAYOUT_GENERAL);

	// intentionally don't set TLAS, it's not build yet at this point!
	set_raytrace = renderer.descriptor_pool.allocate(renderer.layout_raytrace);
	set_raytrace.view(1, renderer.attachment_albedo.getView(), VK_IMAGE_LAYOUT_GENERAL);
	set_raytrace.buffer(2, uniform_buffer, sizeof(SceneUniform));
	set_raytrace.view(6, renderer.attachment_illumination.getView(), VK_IMAGE_LAYOUT_GENERAL);
	set_raytrace.view(7, renderer.attachment_prev_illumination.getView(), VK_IMAGE_LAYOUT_GENERAL);
	set_raytrace.view(8, renderer.attachment_normal.getView(), VK_IMAGE_LAYOUT_GENERAL);
	set_raytrace.view(9, renderer.attachment_prev_depth.getView(), VK_IMAGE_LAYOUT_GENERAL);
	set_raytrace.view(10, renderer.attachment_soild_illumination.getView(), VK_IMAGE_LAYOUT_GENERAL);
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