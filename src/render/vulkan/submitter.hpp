#pragma once

#include "external.hpp"
#include "semaphore.hpp"
#include "fence.hpp"
#include "queue.hpp"

class CommandSubmitter {

	private:

		VkCommandBuffer vk_buffer;
		VkFence finished_fence = VK_NULL_HANDLE;

		std::vector<VkSemaphore> wait_semaphores;
		std::vector<VkPipelineStageFlags> stages;
		std::vector<VkSemaphore> reset_semaphores;

	public:

		CommandSubmitter(VkCommandBuffer vk_buffer);

		CommandSubmitter& awaits(Semaphore& semaphore, VkPipelineStageFlags stage);
		CommandSubmitter& signal(Semaphore& semaphore);
		CommandSubmitter& signal(Fence& fence);
		void done(Queue queue);

};
