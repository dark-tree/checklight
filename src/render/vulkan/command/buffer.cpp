
#include "buffer.hpp"

/*
 * CommandBuffer
 */

CommandBuffer::CommandBuffer(VkCommandPool vk_pool, VkCommandBuffer vk_buffer, VkDevice vk_device)
: vk_pool(vk_pool), vk_buffer(vk_buffer), vk_device(vk_device) {}

CommandRecorder CommandBuffer::record(VkCommandBufferUsageFlags flags) {

	vkResetCommandBuffer(vk_buffer, 0);

	VkCommandBufferBeginInfo info {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.flags = flags;
	info.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(vk_buffer, &info) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to begin recording a command buffer!"};
	}

	return {vk_buffer};

}

CommandSubmitter CommandBuffer::submit() {
	return {vk_buffer};
}

void CommandBuffer::close() {
	vkFreeCommandBuffers(vk_device, vk_pool, 1, &vk_buffer);
}

VkCommandBuffer CommandBuffer::getHandle() const {
	return vk_buffer;
}

/*
 * CommandPool
 */

CommandPool::CommandPool(VkCommandPool vk_pool, VkDevice vk_device)
: vk_pool(vk_pool), vk_device(vk_device) {}

void CommandPool::close() {
	vkDestroyCommandPool(vk_device, vk_pool, nullptr);
}

void CommandPool::reset(bool release) const {
	vkResetCommandPool(vk_device, vk_pool, release ? VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : 0);
}

CommandBuffer CommandPool::allocate(VkCommandBufferLevel level) const {

	VkCommandBufferAllocateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	create_info.commandPool = vk_pool;
	create_info.level = level;
	create_info.commandBufferCount = 1;

	VkCommandBuffer buffer;

	if (vkAllocateCommandBuffers(vk_device, &create_info, &buffer) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to allocate a command buffer!"};
	}

	return {vk_pool, buffer, vk_device};
}

CommandPool CommandPool::create(LogicalDevice& device, const Family& family, bool transient) {

	VkCommandPoolCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	create_info.queueFamilyIndex = family.getIndex();

	if (transient) {
		create_info.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	}

	VkCommandPool pool;

	if (vkCreateCommandPool(device.getHandle(), &create_info, nullptr, &pool) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create command pool!"};
	}

	return {pool, device.getHandle()};

}
