
#include "memory.hpp"

#define PRNTATR(str) printf("   │  - " str "\n")

void Memory::printType(int index) {
	VkMemoryType type = vk_properties.memoryTypes[index];
	VkMemoryPropertyFlags flags = type.propertyFlags;

	int count = std::popcount(flags);

	if (flags == 0) {
		printf("   ├ Type #%d, %d attributes\n", index, count);
	} else {
		if (count == 1) {
			printf("   ├ Type #%d, %d attribute:\n", index, count);
		} else {
			printf("   ├ Type #%d, %d attributes:\n", index, count);
		}
	}

	if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) PRNTATR("VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT");
	if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) PRNTATR("VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");
	if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) PRNTATR("VK_MEMORY_PROPERTY_HOST_COHERENT_BIT");
	if (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) PRNTATR("VK_MEMORY_PROPERTY_HOST_CACHED_BIT");
	if (flags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) PRNTATR("VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT");
	if (flags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD) PRNTATR("VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD");
	if (flags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD) PRNTATR("VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD");
	if (flags & VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV) PRNTATR("VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV");
}

void Memory::printHeap(int index) {
	VkMemoryHeap heap = vk_properties.memoryHeaps[index];
	printf(" * Memory heap #%d of size %lu MiB\n", index, heap.size / 1024 / 1024);

	for (int i = 0; i < vk_properties.memoryTypeCount; i ++) {
		VkMemoryType type = vk_properties.memoryTypes[i];

		if (type.heapIndex == index) {
			printType(i);
		}
	}
}

Memory::Memory(PhysicalDevice& device) {
vkGetPhysicalDeviceMemoryProperties(device.getHandle(), &vk_properties);
}

void Memory::print() {
	printf("INFO: There are %d memory types and %d memory heaps:\n", vk_properties.memoryTypeCount, vk_properties.memoryHeapCount);

	for (uint32_t i = 0; i < vk_properties.memoryHeapCount; i ++) {
		printHeap(i);
		printf("\n");
	}
}