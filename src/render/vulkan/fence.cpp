
#include "fence.hpp"

Fence::Fence(VkDevice vk_device, bool signaled)
: vk_device(vk_device) {

	VkFenceCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	if (signaled) {
		create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	}

	if (vkCreateFence(vk_device, &create_info, nullptr, &vk_fence) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create a fence!"};
	}
}

void Fence::close() {
	vkDestroyFence(vk_device, vk_fence, nullptr);
}

void Fence::wait(size_t timeout) {
	vkWaitForFences(vk_device, 1, &vk_fence, VK_TRUE, timeout);
}

void Fence::reset() {
	vkResetFences(vk_device, 1, &vk_fence);
}

void Fence::lock(size_t timeout) {
	wait(timeout);
	reset();
}

VkFence Fence::getHandle() const {
	return vk_fence;
}