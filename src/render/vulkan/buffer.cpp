
#include "buffer.hpp"

/*
 * Buffer
 */

Buffer::Buffer(VkBuffer vk_buffer, Allocation allocation)
: vk_buffer(vk_buffer), allocation(allocation) {}

void Buffer::close() {
	allocation.closeBuffer(vk_buffer);
}

VkBuffer Buffer::getHandle() const {
	return vk_buffer;
}

Allocation& Buffer::getAllocation() {
	return allocation;
}