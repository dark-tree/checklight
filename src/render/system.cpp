
#include "system.hpp"

/*
 * RenderSystem
 */

std::unique_ptr<Renderer> RenderSystem::renderer {nullptr};

Renderer& RenderSystem::getRenderer() {
	return *renderer;
}

Window& RenderSystem::getWindow() {
	return getRenderer().getWindow();
}

void RenderSystem::initialize(ApplicationParameters& parameters) {
	renderer.reset(new Renderer {parameters});
}

void RenderSystem::terminate() {
	renderer.reset(0);
}