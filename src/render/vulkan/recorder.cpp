
#include "recorder.hpp"

CommandRecorder::CommandRecorder(VkCommandBuffer vk_buffer)
: vk_buffer(vk_buffer) {}

void CommandRecorder::done() {
	if (vkEndCommandBuffer(vk_buffer) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to record a command buffer!"};
	}
}