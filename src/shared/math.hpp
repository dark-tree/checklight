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

}
