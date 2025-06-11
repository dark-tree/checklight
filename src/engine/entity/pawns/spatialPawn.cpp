#include "spatialPawn.hpp"

/*
 * SpatialPawn
 */

SpatialPawn::SpatialPawn() : Pawn() {
	position = {0, 0, 0};
	rotation = {1, 0, 0, 0};
	velocity = {0, 0, 0};
	angular_velocity = {0, 0, 0};
	scale = {1, 1, 1};
}

void SpatialPawn::addPosition(const glm::vec3 new_position) {
	position = position + new_position;
}

void SpatialPawn::setPosition(const glm::vec3 new_position) {
	position = new_position;
}

glm::vec3 SpatialPawn::getPosition() const {
	return position;
}

void SpatialPawn::setVelocity(const glm::vec3 velocity) {
	this->velocity = velocity;
}

glm::vec3 SpatialPawn::getVelocity() {
	return velocity;
}

void SpatialPawn::setAngularVelocity(const glm::vec3 velocity) {
	this->angular_velocity = velocity;
}

glm::vec3 SpatialPawn::getAngularVelocity() {
	return angular_velocity;
}

void SpatialPawn::setRotation(const glm::quat new_rotation) {
	rotation = new_rotation;
}

glm::quat SpatialPawn::getRotation() const {
	return rotation;
}

void SpatialPawn::setScale(const glm::vec3 new_scale) {
	scale = new_scale;
}

glm::vec3 SpatialPawn::getScale() const {
	return scale;
}

glm::mat4x3 SpatialPawn::getMatrix() const {
	const glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
	const glm::mat4 rotMatrix = glm::toMat4(rotation);
	const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	const glm::mat4 affineMatrix = translation * rotMatrix * scaleMatrix;
	return glm::mat4x3(affineMatrix);
}

glm::vec3 SpatialPawn::getForwardVector() const {
	return glm::normalize(math::calculateForwardVector(rotation));
}
