
#include "render/system.hpp"
#include "render/api/vertex.hpp"
#include "render/api/mesh.hpp"
#include "render/api/importer.hpp"

int main() {

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();

	auto meshes = Importer::importObj(system, "assets/models/checklight.obj");

	while (!window.shouldClose()) {
		window.poll();

		float width = system.width();
		float height = system.height();

		glm::mat4 model = glm::identity<glm::mat4>();
		glm::mat4 projection = glm::perspective(glm::radians(40.0f), width / height, 0.1f, 1000.0f);
		projection[1][1] *= -1;
		glm::mat4 view = glm::lookAt(glm::vec3(20.0f, 1.0f, 4.0f), glm::vec3(-3.0f, 1.0f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		system.setProjectionMatrix(projection);
		system.setViewMatrix(view);
		system.setModelMatrix(model);

		system.beginDraw();

		for (auto& mesh : meshes) {
			system.drawMesh(mesh);
		}

		system.endDraw();

		// TODO: Silnik do gier w oparciu o bibliotekę Vulkan
    }

	// close mesh before render systems dies
	system.wait();

	for (auto& mesh : meshes) {
		mesh.reset();
	}

}
