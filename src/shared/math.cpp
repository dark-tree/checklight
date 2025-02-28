#pragma once
#include "math.hpp"

const glm::vec3 math::FORWARD = { 1.0,0.0,0.0 };

glm::vec3 math::quaternionRotation(glm::quat q, glm::vec3 vec) {
	glm::vec3 xyz = glm::vec3(q.x, q.y, q.z);

	return vec + glm::cross(xyz, cross(xyz, vec) + q.w * vec) * 2.0f;
}

glm::vec3 math::calculateForwardVector(glm::quat rotation) {
	return quaternionRotation(rotation, FORWARD);
}

glm::vec2 calculateForwardVector2D(float angle) {
	return {cos(angle), sin(angle)};
}

glm::mat4 math::translationVectorToMatrix(glm::vec3 vector) {
	return glm::mat4(
		1.0f, 0.0f, 0.0f, vector.x,
		0.0f, 1.0f, 0.0f, vector.y,
		0.0f, 0.0f, 1.0f, vector.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}