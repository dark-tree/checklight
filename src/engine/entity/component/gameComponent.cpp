#include "gameComponent.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"

/*
 * GameComponent
 */

bool GameComponent::checkValidity(const Pawn& p) {
	if (dynamic_cast<const SpatialPawn*>(&p)) {
		return true;
	}
	else {
		printf("ERROR: object %s does not inherit \"SpatialPawn\" whitch is required by component %s", p.getPawnName().c_str(), this->getComponentName().c_str());
		return false;
	}
}

SpatialPawn* GameComponent::getSpatialParent() {
	return (SpatialPawn*) parent;
}

glm::vec3 GameComponent::getPosition() {
	return getSpatialParent()->getPosition();
}

glm::quat GameComponent::getRotation() {
	return getSpatialParent()->getRotation();
}

void GameComponent::setPosition(glm::vec3 position) {
	return getSpatialParent()->setPosition(position);
}

void GameComponent::setRotation(glm::quat rotation) {
	return getSpatialParent()->setRotation(rotation);
}

glm::vec3 GameComponent::getForwardVector() {
	return getSpatialParent()->getForwardVector();
}

