#pragma once

#include "external.hpp"

class Queue {

	private:

		VkQueue vk_queue;

	public:

		Queue() = default;
		Queue(VkQueue queue);

		/**
		 * @brief Submit command buffer for execution
		 * @see CommandSubmitter
		 */
		void submit(VkSubmitInfo& info, VkFence fence);

		bool present(VkPresentInfoKHR& info);
};
