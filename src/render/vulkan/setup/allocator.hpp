#pragma once

#include "external.hpp"
#include "device.hpp"
#include "instance.hpp"

class AccelStruct;
class Buffer;
class Image;

enum struct Memory {
	HOST,
	DEVICE,
	SHARED,
	STAGED
};

/**
 * Represents a single memory region, but no necessarily
 * a single Vulkan Allocation.
 */
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

/**
 * Wrapped around the vulkan memory allocation library, can be used
 * to create buffers and images.
 */
class Allocator {

	private:

		VkDevice vk_device;
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
		 *
		 * @param[in] memory The kind of memory heap to use
		 * @param[in] bytes  The minimal number of bytes to allocate
		 * @param[in] usage  Combination of vulkan buffer usage flags
		 * @param[in] name   (Optional) name for the buffer
		 */
		Buffer allocateBuffer(Memory memory, size_t bytes, VkBufferUsageFlags usage, NULLABLE const char* name);

		/**
		 * @brief Allocate image of given size
		 * Allocates a new Vulkan Image with the specified memory properties
		 *
		 * @param[in] memory The kind of memory heap to use
		 * @param[in] width  The image width in pixels
		 * @param[in] height The image height in pixels
		 * @param[in] format The format of a single pixel in the image
		 * @param[in] usage  Combination of vulkan image usage flags
		 * @param[in] layers Number of image array elements
		 * @param[in] levels Number om image mip-map levels
		 * @param[in] name   (Optional) name for the buffer
		 */
		Image allocateImage(Memory memory, int width, int height, VkFormat format, VkImageUsageFlags usage, int layers, int levels, NULLABLE const char* name);

		/**
		 * @brief Allocate raytracing acceleration structure of given size
		 * Allocates a new Vulkan Acceleration Structure, either Top or Bottom, and it's corresponding buffer
		 *
		 * @param[in] type  The kind of acceleration structure to create (BLAS vs. TLAS)
		 * @param[in] bytes The minimal number of bytes to allocate
		 * @param[in] name  (Optional) name for the buffer
		 */
		AccelStruct allocateAcceleration(VkAccelerationStructureTypeKHR type, size_t bytes, NULLABLE const char* name);

};
