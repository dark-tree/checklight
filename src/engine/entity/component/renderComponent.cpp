#include "renderComponent.hpp"
#include "engine/entity/context.hpp"
#include "render/system.hpp"
#include "engine/data/models.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"

RenderComponent::RenderComponent(Models::Shape s) {
	renderObject = RenderSystem::system->createRenderObject();
	renderObject->setMatrix(glm::identity<glm::mat4x3>());
	renderObject->setModel(Models::getShape(s));
}

void RenderComponent::onUpdate(Context c) {
	renderObject->setMatrix(dynamic_cast<SpatialPawn*>(parent)->getMatrix()); //TODO if move byte
}

void RenderComponent::onFixedUpdate(FixedContext c) {

}

InputResult RenderComponent::onEvent(const InputEvent &event) {
	return InputResult::PASS;
}


