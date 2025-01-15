#pragma once

#include "external.hpp"

class Fence {

	public:

		VkDevice vk_device;
		VkFence vk_fence;

	public:

		Fence(VkDevice vk_device, bool signaled = false);

		void close();
		void wait(size_t timeout = UINT64_MAX);
		void reset();
		void lock(size_t timeout = UINT64_MAX);

		VkFence getHandle() const;

};
