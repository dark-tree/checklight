#include "spatialPawn.hpp"

/*
 * SpatialPawn
 */

SpatialPawn::SpatialPawn() {
	position = {0,0,0};
	rotation = {1,0,0,0};
	scale = {1,1,1};
}

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

glm::mat4x3 SpatialPawn::getMatrix() const {
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rotMatrix = glm::toMat4(rotation);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 affineMatrix = translation * rotMatrix * scaleMatrix;
	return glm::mat4x3(affineMatrix); //TODO
}

glm::vec3 SpatialPawn::getForwardVector() {
	return math::calculateForwardVector(rotation);
}


