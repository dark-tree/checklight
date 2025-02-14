
#include "render/system.hpp"
#include "render/api/vertex.hpp"
#include "render/api/mesh.hpp"
#include "render/model/importer.hpp"

int main() {

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();

	auto meshes = Importer::importObj(system, "assets/models/checklight.obj");

	auto model = system.createRenderModel(meshes);

	auto commander = system.createTransientCommander();
	system.rebuildBottomLevel(commander->getRecorder());
	commander->complete();

	// TODO: free meshes
	// for (auto& mesh : meshes) {
	//	  mesh.reset();
	// }

	auto object = system.createRenderObject();
	object->setShader(0);
	object->setMatrix(glm::identity<glm::mat4x3>());
	object->setModel(model);

	while (!window.shouldClose()) {
		window.poll();

		// update uniforms
		// do this once at the beginning of frame rendering
		system.setProjectionMatrix(40.0f, 0.1f, 1000.0f);
		system.setViewMatrix({18.0f, 1.0f, 4.0f}, {-21.0f, 0.0f, 4.0f});
		system.updateUniforms();

		// all rendering must be done between beginDraw() and endDraw()
		system.beginDraw();

		// each frame must end with this call or the engine will deadlock
		system.endDraw();
    }

	system.wait();

	RenderSystem::system.reset();

}
