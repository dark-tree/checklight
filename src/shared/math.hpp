#pragma once
#include "external.hpp"

namespace math {

	extern const glm::vec3 FORWARD;

	glm::vec3 quaternionRotation(glm::quat q, glm::vec3 vec);

	glm::vec3 calculateForwardVector(glm::quat rotation);

	glm::mat4 translationVectorToMatrix(glm::vec3 vector);
}

