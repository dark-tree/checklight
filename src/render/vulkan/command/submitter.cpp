
#include "submitter.hpp"

CommandSubmitter::CommandSubmitter(VkCommandBuffer vk_buffer)
: vk_buffer(vk_buffer) {}

CommandSubmitter& CommandSubmitter::awaits(const Semaphore& semaphore, VkPipelineStageFlags stage) {
	wait_semaphores.push_back(semaphore.getHandle());
	stages.push_back(stage);
	return *this;
}

CommandSubmitter& CommandSubmitter::signal(const Semaphore& semaphore) {
	reset_semaphores.push_back(semaphore.getHandle());
	return *this;
}

CommandSubmitter& CommandSubmitter::signal(const Fence& fence) {
	finished_fence = fence.getHandle();
	return *this;
}

void CommandSubmitter::done(Queue queue) {

	VkSubmitInfo info {};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	info.commandBufferCount = 1;
	info.pCommandBuffers = &vk_buffer;

	// wait for semaphores
	info.waitSemaphoreCount = (uint32_t) wait_semaphores.size();
	info.pWaitSemaphores = wait_semaphores.data();
	info.pWaitDstStageMask = stages.data();

	// unlock semaphore
	info.signalSemaphoreCount = (uint32_t) reset_semaphores.size();
	info.pSignalSemaphores = reset_semaphores.data();

	queue.submit(info, finished_fence);
}