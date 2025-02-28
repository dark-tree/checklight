#include "spatialPawn.hpp"

/*
 * SpatialPawn
 */

void SpatialPawn::addPosition(glm::vec3 new_position) {
	position = position + new_position;
}

void SpatialPawn::setPosition(glm::vec3 new_position) {
	position = new_position;
}

glm::vec3 SpatialPawn::getPosition() const {
	return position;
}

void SpatialPawn::setRotation(glm::quat new_rotation) {
	rotation = new_rotation;
}

glm::quat SpatialPawn::getRotation() const {
	return rotation;
}

void SpatialPawn::setScale(glm::vec3 new_scale) {
	scale = new_scale;
}

glm::vec3 SpatialPawn::getScale() const {
	return scale;
}

glm::mat3x4 SpatialPawn::getMatrix() const {
	return result; //TODO
}

glm::vec3 SpatialPawn::getForwardVector() {
	return math::calculateForwardVector(rotation);
}