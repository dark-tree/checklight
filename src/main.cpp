
#include "render/system.hpp"
#include "render/api/vertex.hpp"
#include "render/api/mesh.hpp"

int main() {

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game");
	parameters.setDimensions(200, 200);

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

		system.beginDraw();
		system.drawMesh(mesh);
		system.endDraw();

		// TODO: Silnik do gier w oparciu o bibliotekę Vulkan
    }

	// close mesh before render systems dies
	system.wait();
	mesh.reset();

}
