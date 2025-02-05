#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/buffer.hpp"

class AccelerationStructure {

	private:

		Buffer buffer;
		VkAccelerationStructureKHR vk_structure;

	public:

		AccelerationStructure(const Buffer& buffer, VkAccelerationStructureKHR vk_structure);

};

