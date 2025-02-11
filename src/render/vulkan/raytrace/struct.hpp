#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/buffer.hpp"

class TraceStruct {

	private:

		Buffer buffer;
		VkAccelerationStructureKHR vk_structure;

	public:

		TraceStruct(const Buffer& buffer, VkAccelerationStructureKHR vk_structure);

		VkAccelerationStructureKHR getHandle() const;

};

