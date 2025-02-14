
#include "frame.hpp"
#include "renderer.hpp"

/*
 * RenderFrame
 */

RenderFrame::RenderFrame(Renderer& renderer, const CommandPool& pool, const LogicalDevice& device)
: buffer(pool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY)), available_semaphore(device.getHandle()), finished_semaphore(device.getHandle()), flight_fence(device.getHandle(), true) {
	uniform_buffer = renderer.allocator.allocateBuffer(Memory::SHARED, sizeof(SceneUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, "Frame Uniform");
	uniform_map = static_cast<SceneUniform*>(uniform_buffer.getAllocation().map());

	set_graphics = renderer.descriptor_pool.allocate(renderer.layout_geometry);
	set_graphics.buffer(0, uniform_buffer, sizeof(SceneUniform));

	set_raytrace = renderer.descriptor_pool.allocate(renderer.layout_raytrace);
	set_raytrace.structure(0, renderer.tlas);
	set_raytrace.view(1, renderer.attachment_albedo.getView());
}

RenderFrame::~RenderFrame() {
	uniform_buffer.getAllocation().unmap();
	uniform_buffer.close();
	buffer.close();
	available_semaphore.close();
	finished_semaphore.close();
	flight_fence.close();
}

void RenderFrame::flushUniformBuffer(const SceneUniform& uniforms) {
	memcpy(uniform_map, &uniforms, sizeof(SceneUniform));
}

void RenderFrame::wait() {
	flight_fence.sync();
}

void RenderFrame::execute() {
	queue.execute();
}