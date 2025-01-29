
#include "render/system.hpp"
#include "render/api/vertex.hpp"
#include "render/api/mesh.hpp"

int main() {

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();

	std::vector<Vertex2D> vertices = {
		{0.0, -0.5,  1.0,  0.0,  0.0},
		{0.5,  0.5,  0.0,  1.0,  0.0},
		{-0.5,  0.5,  0.0,  0.0,  1.0},
	};

	std::vector<int> indices = {
		0,
		1,
		2,
	};

	auto commander = system.createTransientCommander();
	auto mesh = system.createMesh();

	mesh->uploadVertices(*commander, vertices);
	mesh->uploadIndices(*commander, indices);
	commander->complete();

	while (!window.shouldClose()) {
		window.poll();

		float width = system.width();
		float height = system.height();

		glm::mat4 model = glm::identity<glm::mat4>();
		glm::mat4 projection = glm::perspective(glm::radians(65.0f), width / height, 0.1f, 1000.0f);
		glm::mat4 view = glm::lookAt(glm::vec3(0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));;

		system.setProjectionMatrix(projection);
		system.setViewMatrix(view);
		system.setModelMatrix(model);

		system.beginDraw();
		system.drawMesh(mesh);
		system.endDraw();

		// TODO: Silnik do gier w oparciu o bibliotekę Vulkan
    }

	// close mesh before render systems dies
	system.wait();
	mesh.reset();

}
