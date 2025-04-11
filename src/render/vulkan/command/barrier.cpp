
#include "barrier.hpp"

/*
 * MemoryBarrier
 */

MemoryBarrier::MemoryBarrier(VkCommandBuffer buffer)
: buffer(buffer), barrier({}), flags(0), src(0), dst(0) {
	barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	barrier.pNext = nullptr;
}

MemoryBarrier MemoryBarrier::create(VkCommandBuffer buffer) {
	return {buffer};
}

MemoryBarrier& MemoryBarrier::first(VkPipelineStageFlags stage, VkAccessFlags access) {
	this->src = stage;
	this->barrier.srcAccessMask = access;
	return *this;
}

MemoryBarrier& MemoryBarrier::then(VkPipelineStageFlags stage, VkAccessFlags access) {
	this->dst = stage;
	this->barrier.dstAccessMask = access;
	return *this;
}

MemoryBarrier& MemoryBarrier::byRegionBit() {
	flags |= VK_DEPENDENCY_BY_REGION_BIT;
	return *this;
}

void MemoryBarrier::done() {
	vkCmdPipelineBarrier(buffer, src, dst, flags, 1, &barrier, 0, nullptr, 0, nullptr);
}
