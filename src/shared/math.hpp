#pragma once

#include "external.hpp"
#include "ease.hpp"

namespace math {

	extern const glm::vec3 FORWARD;
	extern const glm::vec3 UP;

	glm::vec3 quaternionRotation(glm::quat q, glm::vec3 vec);

	glm::vec3 calculateForwardVector(glm::quat rotation);

	glm::vec2 calculateForwardVector2D(float angle);

	/**
	 * Obtain a vector perpendicular to the given axis, designated by the angle
	 *
	 * @param normal vector perpendicular to the desired vector
	 * @param angle  rotation around the normal
	 * @return perpendicular rotation along normal
	 */
	glm::vec3 rotateAlongAxis(glm::vec3 normal, float angle);

	glm::mat4 translationVectorToMatrix(glm::vec3 vector);

	/**
	 * Approximate the atan2 function
	 *
	 * @param y Value representing the proportion of the y-coordinate.
	 * @param x Value representing the proportion of the x-coordinate.
	 * @return Principal arc tangent of y/x, in the interval [-pi,+pi] radians.
	 */
	float fastAtan2(float y, float x);

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
     * Calculate the value vector of a x b x a
     * @return vector value of a x b x a
     */
    glm::vec3 tripleCrossProduct(glm::vec3& a, glm::vec3& b);

}
