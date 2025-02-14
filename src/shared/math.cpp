
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