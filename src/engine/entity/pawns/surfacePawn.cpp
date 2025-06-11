#include "surfacePawn.hpp"

/*
 * SurfacePawn
 */

SurfacePawn::SurfacePawn(): result() {
	position = {0, 0};
	rotation = 0;
	scale = {1, 1};
}

void SurfacePawn::addPosition(const glm::vec2 new_position) {
	position = position + new_position;
}

void SurfacePawn::setPosition(const glm::vec2 new_position) {
	position = new_position;
}

glm::vec2 SurfacePawn::getPosition() const {
	return position;
}

void SurfacePawn::setRotation(const float new_rotation) {
	rotation = new_rotation;
}

void SurfacePawn::rotateLeft(const float new_rotation) {
	rotation -= new_rotation;
}

void SurfacePawn::rotateRight(const float new_rotation) {
	rotation += new_rotation;
}

float SurfacePawn::getRotation() const {
	return rotation;
}

void SurfacePawn::setScale(const glm::vec2 new_scale) {
	scale = new_scale;
}

glm::vec2 SurfacePawn::getScale() const {
	return scale;
}

glm::mat3x4 SurfacePawn::getMatrix() const {
	return result;
}

glm::vec2 SurfacePawn::getForwardVector() const {
	return math::calculateForwardVector2D(rotation);
}

glm::vec3 SurfacePawn::getForwardVector3D() const {
	const glm::vec2 forward = getForwardVector();
	return glm::vec3{forward.x, forward.y, 0};
}
