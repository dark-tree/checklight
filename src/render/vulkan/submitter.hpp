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

		/// Make the command buffer wait for the given semaphore
		CommandSubmitter& awaits(const Semaphore& semaphore, VkPipelineStageFlags stage);

		/// Make the command buffer signal the given semaphore upon completion
		CommandSubmitter& signal(const Semaphore& semaphore);

		/// Make the command buffer signal the given fence upon completion
		CommandSubmitter& signal(const Fence& fence);

		/// Submit buffer to queue and execute recorded commands
		void done(Queue queue);

};
