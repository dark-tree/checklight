#include "game.hpp"

#include <shared/logger.hpp>

#include "engine/entity/pawns/spatialPawn.hpp"

/*
 * GameComponent
 */


GameComponent::GameComponent(SpatialPawn* t) : OwnedComponent(t) {
}

SpatialPawn* GameComponent::getSpatialParent() const {
	return (SpatialPawn *) parent;
}

glm::vec3 GameComponent::getPosition() const {
	return getSpatialParent()->getPosition();
}

glm::quat GameComponent::getRotation() const {
	return getSpatialParent()->getRotation();
}

void GameComponent::setPosition(const glm::vec3 position) const {
	return getSpatialParent()->setPosition(position);
}

void GameComponent::setRotation(const glm::quat rotation) const {
	return getSpatialParent()->setRotation(rotation);
}

glm::vec3 GameComponent::getForwardVector() const {
	return getSpatialParent()->getForwardVector();
}
