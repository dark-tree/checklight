#include "surfacePawn.hpp"

/*
 * SurfacePawn
 */

void SurfacePawn::addPosition(glm::vec2 new_position) {
	position = position + new_position;
}

void SurfacePawn::setPosition(glm::vec2 new_position) {
	position = new_position;
}

glm::vec2 SurfacePawn::getPosition() const {
	return position;
}

void SurfacePawn::setRotation(float new_rotation) {
	rotation = new_rotation;
}

void SurfacePawn::rotateLeft(float new_rotation) {
	rotation -= new_rotation;
}

void SurfacePawn::rotateRight(float new_rotation) {
	rotation += new_rotation;
}

float SurfacePawn::getRotation() const {
	return rotation;
}

void SurfacePawn::setScale(glm::vec2 new_scale) {
	scale = new_scale;
}

glm::vec2 SurfacePawn::getScale() const {
	return scale;
}

glm::mat3x4 SurfacePawn::getMatrix() const {
	return result; //TODO
}

glm::vec2 SurfacePawn::getForwardVector() {
	return math::calculateForwardVector2D(rotation);
}

glm::vec3 SurfacePawn::getForwardVector3D() {
	glm::vec2 forward = getForwardVector();
	return glm::vec3{ forward.x, forward.y, 0 };
}