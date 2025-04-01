#include "renderComponent.hpp"
#include "engine/entity/context.hpp"
#include "render/system.hpp"
#include "engine/data/models.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"

RenderComponent::RenderComponent(Models::Shape s) : GameComponent(){
	renderObject = RenderSystem::system->createRenderObject();
	renderObject->setMatrix(glm::identity<glm::mat4x3>());
	renderObject->setModel(Models::getShape(s));
	renderObject->setActive(false);
	rendering = false;
}

void RenderComponent::onUpdate(Context c) {
	renderObject->setMatrix(dynamic_cast<SpatialPawn*>(parent)->getMatrix()); //TODO if move byte
}

void RenderComponent::onFixedUpdate(FixedContext c) {

}

InputResult RenderComponent::onEvent(const InputEvent &event) {
	return InputResult::PASS;
}

void RenderComponent::onConnected() {
	setRendering(true);
}

void RenderComponent::setRendering(bool is_rendering) {
	if(is_rendering != rendering){
		renderObject->setActive(is_rendering);
	}
	rendering = is_rendering;
}




