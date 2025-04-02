#pragma once

#include "external.hpp"
#include "render/vulkan/setup/allocator.hpp"

class Buffer {

	private:

		size_t bytes = 0;
		VkBuffer vk_buffer;
		Allocation allocation;

	public:

		Buffer() = default;
		Buffer(VkBuffer vk_buffer, Allocation allocation, size_t bytes);

		void close();

		/// Get underlying vulkan handle
		VkBuffer getHandle() const;

		/// Get memory region control object
		Allocation& getAllocation();

		/// Checks if this buffer is allocated
		bool allocated() const;

		/// get buffer size, in bytes
		size_t size() const;

};
