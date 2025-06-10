#include "render.hpp"
#include "engine/entity/context.hpp"
#include "render/system.hpp"
#include "engine/data/models.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"


RenderComponent::RenderComponent(SpatialPawn* sp, Models::Shape s) : GameComponent(sp) {
	render_object = RenderSystem::system->createRenderObject();
	render_object->setMatrix(glm::identity<glm::mat4x3>());
	render_object->setModel(Models::getShape(s));
	render_object->setActive(false);
	rendering = false;
}

void RenderComponent::onUpdate(Context c) {
	render_object->setMatrix(dynamic_cast<SpatialPawn *>(parent)->getMatrix()); //TODO if move byte
}

void RenderComponent::onFixedUpdate(FixedContext c) {
}

InputResult RenderComponent::onEvent(const InputEvent& event) {
	return InputResult::PASS;
}

void RenderComponent::onConnected() {
	setRendering(true);
}

void RenderComponent::setRendering(bool is_rendering) {
	if (is_rendering != rendering) {
		render_object->setActive(is_rendering);
	}
	rendering = is_rendering;
}

void RenderComponent::remove() {
	GameComponent::remove();
	render_object->setActive(false);
}

RenderComponent::~RenderComponent() {
}
