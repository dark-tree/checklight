#pragma once

#include "external.hpp"

namespace math {

	/**
	 * Convert glm::mat4x3 to the corresponding vulkan matrix class VkTransformMatrixKHR
	 *
	 * @param matrix GLM affine transform matrix
	 * @return Vulkan affine transform matrix
	 */
	VkTransformMatrixKHR toVulkanAffine(const glm::mat4x3& matrix);

	/**
	 * Round up size to the next alignment
	 *
	 * @param size      Size of the elements to be aligned
	 * @param alignment Target value alignment
	 * @return Value rounded up to the next alignment
	 */
	size_t alignUp(size_t size, size_t alignment);

	/**
	 * Obtain a vector perpendicular to the given axis, designated by the angle
	 *
	 * @param normal vector perpendicular to the desired vector
	 * @param angle  rotation around the normal
	 * @return perpendicular rotation along normal
	 */
	glm::vec3 rotateAlongAxis(glm::vec3 normal, float angle);

}
