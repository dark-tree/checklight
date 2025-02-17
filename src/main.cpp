
#include "render/system.hpp"
#include "render/model/importer.hpp"
#include "input/input.hpp"
#include "engine/engine.hpp"

int main() {

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game!");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();


	window.getInputDispatcher().registerListener(std::make_shared<DebugInputListener>());

	SceneManager sceneManager;

	auto meshes = Importer::importObj(system, "assets/models/checklight.obj");

	while (!window.shouldClose()) {
		window.poll();

		//physics update before rendering
		sceneManager.update();

		// update uniforms
		// do this once at the beginning of frame rendering
		system.setProjectionMatrix(40.0f, 0.1f, 1000.0f);
		system.setViewMatrix(sceneManager.getMainScene()->getMainCam().getPosition(), sceneManager.getMainScene()->getMainCam().forwardVector());
		system.updateUniforms();

		// all rendering must be done between beginDraw() and endDraw()
		system.beginDraw();

		// this does nothing for now, this operation will most likely be expensive later
		system.bindMaterial();

		for (auto& mesh : meshes) {
			glm::mat4 model = glm::identity<glm::mat4>();

			// first bind the mesh
			system.bindMesh(mesh);

			// then you can draw it multiple times
			system.draw(model);

			// later using a precomputed position vector may be reqired by the renderer
			// take that into account
		}

		// each frame must end with this call or the engine will deadlock
		system.endDraw();
    }

	system.wait();

	for (auto& mesh : meshes) {
		mesh.reset();
	}

	RenderSystem::system.reset();

}
