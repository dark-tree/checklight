
#include "buffer.hpp"

/*
 * Buffer
 */

Buffer::Buffer(VkBuffer vk_buffer, Allocation allocation)
: vk_buffer(vk_buffer), allocation(allocation) {}

VkBuffer Buffer::getHandle() {
	return vk_buffer;
}

Allocation& Buffer::getAllocation() {
	return allocation;
}