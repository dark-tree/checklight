#pragma once

#include "external.hpp"

/**
 * Allows forcing the GPU to wait for the GPU to finish some operation,
 * for inter GPU-CPU locking see Fence.
 */
class Semaphore {

	private:

		VkDevice vk_device;
		VkSemaphore vk_semaphore;

	public:

		Semaphore(VkDevice vk_device);

		void close();
		VkSemaphore getHandle() const;

};
