
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

	auto commander = system.createTransientCommander();
	auto mesh = system.createMesh();

	mesh->upload(*commander, vertices);
	commander->complete();

	// this will change later
	system.addForRendering(mesh);

	while (!window.shouldClose()) {
		window.poll();
		system.draw();

		// TODO: Silnik do gier w oparciu o bibliotekę Vulkan
    }

	system.close();
}
