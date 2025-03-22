
#include "allocator.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/vulkan/buffer/image.hpp"
#include "render/vulkan/raytrace/struct.hpp"
#include "render/vulkan/setup/proxy.hpp"
#include "debug.hpp"

/*
 * Allocation
 */

Allocation::Allocation(VmaAllocator allocator, VmaAllocation allocation)
: vma_allocator(allocator), vma_allocation(allocation) {}

void Allocation::closeBuffer(VkBuffer buffer) {
	if (!empty()) {
		VulkanDebug::endLifetime(buffer);
		vmaDestroyBuffer(vma_allocator, buffer, vma_allocation);
		vma_allocation = VK_NULL_HANDLE;
	}
}

void Allocation::closeImage(VkImage image) {
	if (!empty()) {
		VulkanDebug::endLifetime(image);
		vmaDestroyImage(vma_allocator, image, vma_allocation);
		vma_allocation = VK_NULL_HANDLE;
	}
}

void* Allocation::map() {
	void* pointer;
	vmaMapMemory(vma_allocator, vma_allocation, &pointer);
	return pointer;
}

void Allocation::unmap() {
	vmaUnmapMemory(vma_allocator, vma_allocation);
}

void Allocation::flushNonCoherent(size_t offset, size_t size) {
	vmaFlushAllocation(vma_allocator, vma_allocation, offset, size);
}

bool Allocation::empty() const {
	return vma_allocation == VK_NULL_HANDLE;
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

	vk_device = logical.getHandle();

	VmaVulkanFunctions functions = {};
	functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo create_info {};
	create_info.physicalDevice = physical.getHandle();
	create_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	create_info.device = logical.getHandle();
	create_info.pVulkanFunctions = &functions;
	create_info.instance = instance.getHandle();
	create_info.vulkanApiVersion = VK_API_VERSION_1_0;
	create_info.pAllocationCallbacks = nullptr;

	if (vmaCreateAllocator(&create_info, &vma_allocator) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create allocator!"};
	}

	vkGetPhysicalDeviceMemoryProperties(physical.getHandle(), &vk_properties);
}

void Allocator::close() {
	vmaDestroyAllocator(vma_allocator);
}

void Allocator::print() {
	out::info("There are %d memory types and %d memory heaps", vk_properties.memoryTypeCount, vk_properties.memoryHeapCount);

	size_t free = 0;

	for (uint32_t i = 0; i < vk_properties.memoryHeapCount; i ++) {
		VkMemoryHeap heap = vk_properties.memoryHeaps[i];

		if (heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
			free += heap.size / 1024 / 1024;
		}
	}

	out::info("Found %lu MiB of device local free memory", free);
}

Buffer Allocator::allocateBuffer(Memory memory, size_t bytes, VkBufferUsageFlags usage, const char* name) {
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

	if (!name) {
		name = "Unnamed";
	}

	VulkanDebug::beginLifetime(VK_OBJECT_TYPE_BUFFER, buffer, name);
	VulkanDebug::setDebugName(vk_device, VK_OBJECT_TYPE_BUFFER, buffer, name);
	return {buffer, {vma_allocator, allocation}, bytes};
}

Image Allocator::allocateImage(Memory memory, int width, int height, VkFormat format, VkImageUsageFlags usage, int layers, int levels, VkSampleCountFlagBits samples, const char* name) {
	VmaAllocationCreateInfo allocation_info {};
	fromMemoryGroup(&allocation_info, memory);

	VkImageCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	create_info.imageType = VK_IMAGE_TYPE_2D;
	create_info.extent.width = width;
	create_info.extent.height = height;
	create_info.extent.depth = 1;
	create_info.mipLevels = levels;
	create_info.arrayLayers = layers;
	create_info.format = format;
	create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	create_info.usage = usage;
	create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create_info.samples = samples;
	create_info.flags = 0;

	VkImage image;
	VmaAllocation allocation;

	if (vmaCreateImage(vma_allocator, &create_info, &allocation_info, &image, &allocation, nullptr) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to allocate image!"};
	}

	if (!name) {
		name = "Unnamed";
	}

	VulkanDebug::beginLifetime(VK_OBJECT_TYPE_IMAGE, image, name);
	VulkanDebug::setDebugName(vk_device, VK_OBJECT_TYPE_IMAGE, image, name);
	return {image, format, {vma_allocator, allocation}};
}

AccelStruct Allocator::allocateAcceleration(VkAccelerationStructureTypeKHR type, size_t bytes, const char* name) {

	// the second flag is needed by TLAS to link with BLASes
	VkBufferUsageFlags flags = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	Buffer backing = allocateBuffer(Memory::DEVICE, bytes, flags, name);

	VkAccelerationStructureCreateInfoKHR create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	create_info.type = type;
	create_info.size = bytes;
	create_info.offset = 0; // offset into buffer
	create_info.buffer = backing.getHandle();
	create_info.deviceAddress = 0;

	VkAccelerationStructureKHR structure;

	if (Proxy::vkCreateAccelerationStructureKHR(vk_device, &create_info, nullptr, &structure) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to allocate acceleration structure!"};
	}

	VulkanDebug::beginLifetime(VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, structure, name);
	VulkanDebug::setDebugName(vk_device, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, structure, name);

	return {backing, structure};

}