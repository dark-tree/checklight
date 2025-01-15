#pragma once

#include "external.hpp"
#include "allocator.hpp"

class Buffer {

	private:

		VkBuffer vk_buffer;
		Allocation allocation;

	public:

		Buffer() = default;
		Buffer(VkBuffer vk_buffer, Allocation allocation);

		VkBuffer getHandle();
		Allocation& getAllocation();

};
