
#include "buffer.hpp"

/*
 * Buffer
 */

Buffer::Buffer(VkBuffer vk_buffer, Allocation allocation, size_t bytes)
: bytes(bytes), vk_buffer(vk_buffer), allocation(allocation) {}

void Buffer::close() {
	allocation.closeBuffer(vk_buffer);
}

VkBuffer Buffer::getHandle() const {
	return vk_buffer;
}

Allocation& Buffer::getAllocation() {
	return allocation;
}

bool Buffer::allocated() const {
	return !allocation.empty();
}

size_t Buffer::size() const {
	return bytes;
}