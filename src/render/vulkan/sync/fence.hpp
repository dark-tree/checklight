#pragma once

#include "external.hpp"

/**
 * Allows forcing the CPU to wait for the GPU to finish some operation,
 * for inter-GPU locking see Semaphore.
 */
class Fence {

	private:

		VkDevice vk_device;
		VkFence vk_fence;

	public:

		Fence(VkDevice vk_device, bool signaled = false);

		void close();

		/**
		 * @brief Wait for the fence to unlock
		 * @param timeout Optional, maximum timeout in nanoseconds
		 */
		void wait(size_t timeout = UINT64_MAX);

		/**
		 * @brief Re-lock the fence
		 * This needs to be called if you wish to reuse the fence
		 */
		void reset();

		/**
		 * @brief Combination of wait() and reset()
		 * @param timeout Optional, maximum timeout in nanoseconds
		 */
		void sync(size_t timeout = UINT64_MAX);

		/**
		 * @brief Get underlying vulkan handle
		 * Try avoiding using this when possible
		 */
		VkFence getHandle() const;

};
