
#include "math.hpp"

const glm::vec3 math::FORWARD = { 1.0,0.0,0.0 };
const glm::vec3 math::UP = { 0.0,1.0,0.0 };

glm::vec3 math::quaternionRotation(glm::quat q, glm::vec3 vec) {
	glm::vec3 xyz = glm::vec3(q.x, q.y, q.z);

	return vec + glm::cross(xyz, cross(xyz, vec) + q.w * vec) * 2.0f;
}

glm::vec3 math::calculateForwardVector(glm::quat rotation) {
	return quaternionRotation(rotation, FORWARD);
}

glm::vec2 math::calculateForwardVector2D(float angle) {
	return {cos(angle), sin(angle)};
}

glm::vec3 math::rotateAlongAxis(glm::vec3 normal, float angle) {
	glm::vec3 axis = glm::cross(normal, glm::vec3(1.0f, 0.0f, 0.0f));

	if (glm::length(axis) < 0.01) {
	    axis = glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, normal);
	return glm::normalize(glm::vec3(rotation * glm::vec4(axis, 0.0f)));
}

glm::mat4 math::translationVectorToMatrix(glm::vec3 vector) {
	return glm::mat4(
	        1.0f, 0.0f, 0.0f, vector.x,
	        0.0f, 1.0f, 0.0f, vector.y,
	        0.0f, 0.0f, 1.0f, vector.z,
	        0.0f, 0.0f, 0.0f, 1.0f
	);
}

float math::fastAtan2(float y, float x) {
	float abs_y = std::fabs(y) + 1e-10f; // kludge to prevent 0/0 condition
	float r = (x - std::copysign(abs_y, x)) / (abs_y + std::fabs(x));
	float angle = M_PI/2.f - std::copysign(M_PI/4.f, x);

	angle += (0.1963f * r * r - 0.9817f) * r;
	return std::copysign(angle, y);
}

VkTransformMatrixKHR math::toVulkanAffine(const glm::mat4x3& matrix) {
	VkTransformMatrixKHR transform {};

	for (int row = 0; row < 3; row ++) {
	    for (int column = 0; column < 4; column ++) {
	        transform.matrix[row][column] = matrix[column][row];
	    }
	}

	return transform;
}

size_t math::alignUp(size_t size, size_t alignment) {
	return (size + alignment - 1) & ~(alignment - 1);
}

glm::vec3 tripleCrossProduct(glm::vec3& a, glm::vec3& b)
{
    return glm::cross(glm::cross(a, b), a);
}
