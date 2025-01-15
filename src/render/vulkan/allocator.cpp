
#include "allocator.hpp"
#include "buffer.hpp"
#include "image.hpp"

/*
 * Allocation
 */

Allocation::Allocation(VmaAllocator allocator, VmaAllocation allocation)
: vma_allocator(allocator), vma_allocation(allocation) {}

void Allocation::closeBuffer(VkBuffer buffer) {
	vmaDestroyBuffer(vma_allocator, buffer, vma_allocation);
}

void Allocation::closeImage(VkImage image) {
	vmaDestroyImage(vma_allocator, image, vma_allocation);
}

/*
 * Allocator
 */

void Allocator::fromMemoryGroup(VmaAllocationCreateInfo* create_info, Memory group) {
	if (group == Memory::HOST) {
		create_info->requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		create_info->preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		create_info->flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		create_info->usage = VMA_MEMORY_USAGE_UNKNOWN;
		return;
	}

	if (group == Memory::DEVICE) {
		create_info->requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		create_info->preferredFlags = VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD;
		create_info->flags = 0;
		create_info->usage = VMA_MEMORY_USAGE_UNKNOWN;
		return;
	}

	if (group == Memory::SHARED) {
		create_info->requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		create_info->preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		create_info->flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		create_info->usage = VMA_MEMORY_USAGE_UNKNOWN;
		return;
	}

	if (group == Memory::STAGED) {
		create_info->requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		create_info->preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		create_info->flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		create_info->usage = VMA_MEMORY_USAGE_UNKNOWN;
		return;
	}

	UNREACHABLE;
}

Allocator::Allocator(LogicalDevice& logical, PhysicalDevice& physical, Instance& instance) {
	VmaVulkanFunctions functions = {};
	functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo create_info {};
	create_info.physicalDevice = physical.getHandle();
	create_info.device = logical.getHandle();
	create_info.pVulkanFunctions = &functions;
	create_info.instance = instance.getHandle();
	create_info.vulkanApiVersion = VK_API_VERSION_1_0;
	create_info.pAllocationCallbacks = nullptr;

	if (vmaCreateAllocator(&create_info, &vma_allocator) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create allocator!"};
	}
}

void Allocator::close() {
	vmaDestroyAllocator(vma_allocator);
}

Buffer Allocator::allocateBuffer(Memory memory, size_t bytes, VkBufferUsageFlagBits usage) {
	VmaAllocationCreateInfo allocation_info {};
	fromMemoryGroup(&allocation_info, memory);

	VkBufferCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	create_info.size = bytes;
	create_info.usage = usage;
	create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkBuffer buffer;
	VmaAllocation allocation;

	if (vmaCreateBuffer(vma_allocator, &create_info, &allocation_info, &buffer, &allocation, nullptr) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to allocate buffer!"};
	}

	return {buffer, {vma_allocator, allocation}};
}

Image Allocator::allocateImage(Memory memory, int width, int height, VkFormat format, VkBufferUsageFlagBits usage, int mips) {
	VmaAllocationCreateInfo allocation_info {};
	fromMemoryGroup(&allocation_info, memory);

	VkImageCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	create_info.imageType = VK_IMAGE_TYPE_2D;
	create_info.extent.width = width;
	create_info.extent.height = height;
	create_info.extent.depth = 1;
	create_info.mipLevels = mips;
	create_info.arrayLayers = 1;
	create_info.format = format;
	create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	create_info.usage = usage;
	create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	create_info.flags = 0;

	VkImage image;
	VmaAllocation allocation;

	if (vmaCreateImage(vma_allocator, &create_info, &allocation_info, &image, &allocation, nullptr) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to allocate image!"};
	}

	return {image, format, {vma_allocator, allocation}};
}