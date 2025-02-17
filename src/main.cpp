
#include "render/render.hpp"

int main() {

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();

	auto meshes = Importer::importObj(system, "assets/models/checklight.obj");

	auto models = system.createRenderModels(meshes);

	auto commander = system.createTransientCommander();
	system.rebuildBottomLevel(commander->getRecorder());
	commander->complete();

	std::vector<std::shared_ptr<RenderObject>> objects;

	for (auto& model : models) {
		auto object = system.createRenderObject();
		object->setMatrix(glm::identity<glm::mat4x3>());
		object->setModel(model, system.getDevice());
		objects.push_back(object);
	}
	
	while (!window.shouldClose()) {
		window.poll();

		// update uniforms
		// do this once at the beginning of frame rendering
		system.setProjectionMatrix(40.0f, 0.1f, 1000.0f);
		system.setViewMatrix({18.0f, sin(glfwGetTime()) * 10, 4.0f}, {-21.0f, 0.0f, 4.0f});

		// render the scene
		system.draw();
  }

	system.wait();

	// TODO: free meshes
	//for (auto& mesh : meshes) {
	//	mesh.reset();
	//}

	RenderSystem::system.reset();

}
