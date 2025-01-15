#pragma once

#include "external.hpp"
#include "device.hpp"
#include "instance.hpp"

class Buffer;
class Image;

enum struct Memory {
	HOST,
	DEVICE,
	SHARED,
	STAGED
};

class Allocation {

	private:

		VmaAllocator vma_allocator;
		VmaAllocation vma_allocation;

	public:

		Allocation() = default;
		Allocation(VmaAllocator allocator, VmaAllocation allocation);

		void closeBuffer(VkBuffer buffer);
		void closeImage(VkImage image);

};

class Allocator {

	private:

		VmaAllocator vma_allocator;

		void fromMemoryGroup(VmaAllocationCreateInfo* create_info, Memory group);

	public:

		Allocator() = default;
		Allocator(LogicalDevice& logical, PhysicalDevice& physical, Instance& instance);

		void close();

	public:

		/**
		 * Allocates a new Vulkan Buffer with the specified memory properties
		 */
		Buffer allocateBuffer(Memory memory, size_t bytes, VkBufferUsageFlagBits usage);

		/**
		 * Allocates a new Vulkan Image with the specified memory properties
		 */
		Image allocateImage(Memory memory, int width, int height, VkFormat format, VkBufferUsageFlagBits usage, int mips = 1);

};
