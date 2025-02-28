#include "gameObject.hpp"
#include "spatialObject.hpp"
#include "shared/math.hpp"


SpatialObject::SpatialObject() : GameObject() {
	translation = glm::vec3(0.0f);
	rotation = glm::identity<glm::quat>();
}

SpatialObject::SpatialObject(glm::vec3 pos) : GameObject() {
	translation = pos;
}

SpatialObject::SpatialObject(glm::vec3 pos, glm::quat rot) : GameObject() {
	translation = pos;
	rotation = rot;
}

glm::vec3 SpatialObject::getPosition() const {
	return translation;
}

glm::quat SpatialObject::getRotation() const {
	return rotation;
}

void SpatialObject::setPosition(glm::vec3 newPosition) {
	translation = newPosition;
}

void SpatialObject::setRotation(glm::quat newRotation) {
	rotation = newRotation;
}

glm::vec3 SpatialObject::forwardVector() {
	return math::calculateForwardVector(rotation);
}