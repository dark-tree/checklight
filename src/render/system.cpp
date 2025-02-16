
#include "system.hpp"
#include "api/commander.hpp"
#include "api/mesh.hpp"
#include "api/model.hpp"

/*
 * RenderSystem
 */

std::unique_ptr<RenderSystem> RenderSystem::system {nullptr};

void RenderSystem::init(ApplicationParameters& parameters) {
	RenderSystem::system = std::make_unique<RenderSystem>(parameters);
}

RenderSystem::RenderSystem(ApplicationParameters& parameters)
: Renderer(parameters) {}

std::shared_ptr<RenderModel> RenderSystem::createRenderModel(std::vector<std::shared_ptr<RenderMesh>> meshes) {
	AccelStructConfig config = AccelStructConfig::create(AccelStructConfig::BUILD, AccelStructConfig::BOTTOM);

	for (auto& mesh : meshes) {
		config.addTriangles(device, *mesh, true);
		config.setFlags(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);
		config.setDebugName("<model>"); // TODO
	}

	return bakery.submit(device, allocator, config);
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

void RenderSystem::setProjectionMatrix(float fov, float near_plane, float far_plane) {
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float) width() / (float) height(), near_plane, far_plane);
	projection[1][1] *= -1;

	getFrame().uniforms.projection = projection;
}

void RenderSystem::setViewMatrix(glm::vec3 eye, glm::vec3 direction) {
	getFrame().uniforms.view = glm::lookAt(eye, eye + direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

std::shared_ptr<RenderObject> RenderSystem::createRenderObject() {
	return {instances->create()};
}
