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

		void close();

		/// Get underlying vulkan handle
		VkBuffer getHandle() const;

		/// Get memory region control object
		Allocation& getAllocation();

};
