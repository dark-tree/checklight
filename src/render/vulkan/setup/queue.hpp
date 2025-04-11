#pragma once

#include "external.hpp"

class Queue {

	private:

		VkQueue vk_queue;

	public:

		Queue() = default;
		Queue(VkQueue queue);

		/**
		 * Submit command buffer for execution
		 * @see CommandSubmitter
		 */
		void submit(VkSubmitInfo& info, VkFence fence);

		/**
		 * Present framebuffer image using the queue
		 * @see Swapchain
		 */
		bool present(VkPresentInfoKHR& info);
};
