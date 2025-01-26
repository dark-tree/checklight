
#include "system.hpp"
#include "api/commander.hpp"
#include "api/mesh.hpp"

/*
 * RenderSystem
 */

std::unique_ptr<RenderSystem> RenderSystem::system {nullptr};

void RenderSystem::init(ApplicationParameters& parameters) {
	RenderSystem::system = std::make_unique<RenderSystem>(parameters);
}

void RenderSystem::drawFrame(RenderFrame& frame, CommandRecorder& recorder, uint32_t image) {

	for (auto& mesh : meshes) {

		recorder.beginRenderPass(pass_basic_3d, image, swapchain.getExtend())
			.bindPipeline(pipeline_basic_3d)
			.bindDescriptorSet(frame.set_0)
			.bindVertexBuffer(mesh->getVertexBuffer())
			.draw(mesh->getVertexCount())
			.endRenderPass();

	}

}

RenderSystem::RenderSystem(ApplicationParameters& parameters)
: Renderer(parameters) {}

void RenderSystem::close() {
	wait();
	meshes.clear();
}

std::unique_ptr<RenderCommander> RenderSystem::createTransientCommander() {

	CommandBuffer buffer = transient_pool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	CommandRecorder recorder = buffer.record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// TODO why tf does make_unique not work here?
	return std::unique_ptr<RenderCommander> {new RenderCommander {RenderCommander::TRANSIENT, buffer, recorder, {}}};

}

std::shared_ptr<RenderMesh> RenderSystem::createMesh() {
	return std::make_shared<RenderMesh>();
}

void RenderSystem::addForRendering(std::shared_ptr<RenderMesh>& mesh) {
	meshes.emplace_back(mesh);
}