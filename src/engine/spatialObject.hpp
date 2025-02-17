#pragma once
#include "external.hpp"
#include "gameObject.hpp"


class SpatialObject : public GameObject {
protected:
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 scale;
public:
	SpatialObject();

	SpatialObject(glm::vec3 pos);

	SpatialObject(glm::vec3 pos, glm::quat rot);

	glm::vec3 getPosition() const;

	glm::quat getRotation() const;

	void setPosition(glm::vec3 newPosition);

	void setRotation(glm::quat newRotation);

	glm::vec3 forwardVector();
};