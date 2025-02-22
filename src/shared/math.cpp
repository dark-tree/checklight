
#include "math.hpp"

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

glm::vec3 math::rotateAlongAxis(glm::vec3 normal, float angle) {
	glm::vec3 axis = glm::cross(normal, glm::vec3(1.0f, 0.0f, 0.0f));

	if (glm::length(axis) < 0.01) {
		axis = glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, normal);
	return glm::normalize(glm::vec3(rotation * glm::vec4(axis, 0.0f)));
}