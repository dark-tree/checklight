
#define GLFW_INCLUDE_VULKAN
#include "render/system.hpp"

int main() {

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game");
	parameters.setDimensions(200, 200);

	RenderSystem::initialize(parameters);
	Renderer& renderer = RenderSystem::getRenderer();
	Window& window = RenderSystem::getWindow();

	while (!window.shouldClose()) {
		window.poll();
		renderer.draw();

		// TODO: Silnik do gier w oparciu o bibliotekę Vulkan
    }

	RenderSystem::terminate();
}
