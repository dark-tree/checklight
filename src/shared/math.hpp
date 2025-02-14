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

}
