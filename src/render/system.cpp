
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

RenderSystem::RenderSystem(ApplicationParameters& parameters)
: Renderer(parameters) {}

std::unique_ptr<RenderCommander> RenderSystem::createTransientCommander() {

	CommandBuffer buffer = transient_pool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	CommandRecorder recorder = buffer.record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// TODO why tf does make_unique not work here?
	return std::unique_ptr<RenderCommander> {new RenderCommander {RenderCommander::TRANSIENT, buffer, recorder, {}}};

}

std::shared_ptr<RenderMesh> RenderSystem::createMesh() {
	return std::make_shared<RenderMesh>();
}

void RenderSystem::setProjectionMatrix(glm::mat4 projection) {
	this->projection = projection;
}

void RenderSystem::setViewMatrix(glm::mat4 view) {
	this->view = view;
}

void RenderSystem::setModelMatrix(glm::mat4 model) {
	this->model = model;
}

void RenderSystem::drawMesh(std::shared_ptr<RenderMesh>& mesh) {

	RenderFrame& frame = getFrame();

	SceneUniform uniform;
	uniform.view_projection = projection * view;
	uniform.model = model;

	frame.flushUniformBuffer(uniform);

	recorder.beginRenderPass(pass_basic_3d, current_image, swapchain.getExtend())
		.bindPipeline(pipeline_basic_3d)
		.bindDescriptorSet(frame.set_0);

	mesh->draw(recorder);

	recorder.endRenderPass();

}
