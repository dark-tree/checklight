
#include "queue.hpp"

Queue::Queue(VkQueue queue)
: vk_queue(queue) {}

void Queue::submit(VkSubmitInfo& info, VkFence fence) {
	if (vkQueueSubmit(vk_queue, 1, &info, fence) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to submit command buffer!"};
	}
}

bool Queue::present(VkPresentInfoKHR& info) {
	return vkQueuePresentKHR(vk_queue, &info) != VK_SUCCESS;
}