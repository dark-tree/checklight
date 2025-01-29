
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

		// projection usually stays constant but as it's not too slow
		// and screen size can change do it inside the render loop
		glm::mat4 projection = glm::perspective(glm::radians(65.0f), width / height, 0.1f, 1000.0f);

		// OpenGL correction (don't touch)
		projection[1][1] *= -1;

		// view is based on player/editor camera
		glm::mat4 view = glm::lookAt(glm::vec3(0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));;

		// update uniforms, do this once
		// at the beginning of frame rendering
		system.setProjectionMatrix(projection);
		system.setViewMatrix(view);
		system.updateUniforms();

		// all rendering must be done between beginDraw and endDraw
		system.beginDraw();

		// changing materials is very expensive, try to never bind the same material twice!
		system.bindMaterial();

		float s = (sin(glfwGetTime() * 2) + 1) / 2;

		// once a mesh is bond it can be drawn multiple times quickly
		system.bindMesh(mesh);
		system.draw(glm::scale(glm::identity<glm::mat4>(), glm::vec3(s, s, 1)));

		// at the end of a frame call endDraw or the engine will deadlock
		system.endDraw();
    }

	// close mesh *before* render systems dies
	system.wait();
	mesh.reset();

}
