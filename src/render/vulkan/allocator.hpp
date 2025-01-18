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

		VmaAllocator vma_allocator = VK_NULL_HANDLE;
		VmaAllocation vma_allocation = VK_NULL_HANDLE;

	public:

		Allocation() = default;
		Allocation(VmaAllocator allocator, VmaAllocation allocation);

		void* map();
		void unmap();
		void flushNonCoherent(size_t offset = 0, size_t size = VK_WHOLE_SIZE);
		void closeBuffer(VkBuffer buffer);
		void closeImage(VkImage image);

		bool empty() const;

};

class Allocator {

	private:

		VmaAllocator vma_allocator;
		VkPhysicalDeviceMemoryProperties vk_properties;

		void fromMemoryGroup(VmaAllocationCreateInfo* create_info, Memory group);

	public:

		Allocator() = default;
		Allocator(LogicalDevice& logical, PhysicalDevice& physical, Instance& instance);

		void close();
		void print();

	public:

		/**
		 * @brief Allocate buffer of given size
		 * Allocates a new Vulkan Buffer with the specified memory properties
		 */
		Buffer allocateBuffer(Memory memory, size_t bytes, VkBufferUsageFlags usage);

		/**
		 * @brief Allocate image of given size
		 * Allocates a new Vulkan Image with the specified memory properties
		 */
		Image allocateImage(Memory memory, int width, int height, VkFormat format, VkImageUsageFlags usage, int layers, int levels);

};
