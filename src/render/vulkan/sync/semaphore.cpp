
#include "semaphore.hpp"
#include "shared/logger.hpp"

/*
 * Semaphore
 */

Semaphore::Semaphore(VkDevice vk_device)
: vk_device(vk_device) {

	VkSemaphoreCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(vk_device, &create_info, nullptr, &vk_semaphore) != VK_SUCCESS) {
		FAULT("Failed to create a semaphore!");
	}
}

void Semaphore::close() {
	vkDestroySemaphore(vk_device, vk_semaphore, nullptr);
}

VkSemaphore Semaphore::getHandle() const {
	return vk_semaphore;
}