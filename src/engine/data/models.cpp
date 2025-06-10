#include "models.hpp"

void Models::init() {
	initialized = true;
	RenderSystem& system = *RenderSystem::system;

	models = system.importObj("assets/models/default/shapes.obj");
}

std::shared_ptr<RenderModel> Models::getShape(const Shape s) {
	return models[static_cast<int>(s)];
}

void Models::terminate() {
	models.clear();
}
