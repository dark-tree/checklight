#pragma once

#include "external.hpp"

class MemoryBarrier {

	private:

		VkCommandBuffer buffer;
		VkMemoryBarrier barrier;
		VkDependencyFlags flags;
		VkPipelineStageFlags src, dst;

	public:

		MemoryBarrier(VkCommandBuffer buffer);
		static MemoryBarrier create(VkCommandBuffer buffer);

		MemoryBarrier& first(VkPipelineStageFlags stage, VkAccessFlags access);
		MemoryBarrier& then(VkPipelineStageFlags stage, VkAccessFlags access);
		MemoryBarrier& byRegionBit();

		void done();

};
