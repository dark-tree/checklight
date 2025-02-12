#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/buffer.hpp"

/**
 * Represents a single vulkan Acceleration Structure
 * you create an empty one using the Allocator, or a TLAS/BLAS using the
 * AccelStructFactory and related classes.
 *
 * @verbatim
 *                     ┌──────────────────────────┐
 *                     │    AccelStructFactory    │
 *                     │                          │
 * ┌─────────────────┐ │ ┌──────────────────────┐ │ ┌──────────────────┐
 * │AccelStructConfig├─┼►│AccelStructBakedConfig├─┼►│AccelStruct (BLAS)│
 * └─────────────────┘ │ └──────────────────────┘ │ └──────────────────┘
 * ┌─────────────────┐ │ ┌──────────────────────┐ │ ┌──────────────────┐
 * │AccelStructConfig├─┼►│AccelStructBakedConfig├─┼►│AccelStruct (TLAS)│
 * └─────────────────┘ │ └──────────────────────┘ │ └──────────────────┘
 *                     │           ...            │
 *                     └──────────────────────────┘
 */
class AccelStruct {

	private:

		Buffer buffer;
		VkAccelerationStructureKHR vk_structure;

	public:

		AccelStruct() = default;
		AccelStruct(const Buffer& buffer, VkAccelerationStructureKHR vk_structure);

		void close(const LogicalDevice& device);

		VkAccelerationStructureKHR getHandle() const;

};

