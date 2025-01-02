#pragma once

#include "external.hpp"
#include "device.hpp"
#include "recorder.hpp"

class CommandBuffer {

	public:

		VkCommandPool vk_pool;
		VkCommandBuffer vk_buffer;
		VkDevice vk_device;

	public:

		CommandBuffer() = default;
		CommandBuffer(VkCommandPool vk_pool, VkCommandBuffer vk_buffer, VkDevice vk_device);

		void close();

		/**
		 * TODO documentation
		 */
		CommandRecorder record(VkCommandBufferUsageFlags flags = 0);


};

class CommandPool {

	public:

		VkCommandPool vk_pool;
		VkDevice vk_device;

	public:

		CommandPool() = default;
		CommandPool(VkCommandPool vk_pool, VkDevice vk_device);

		void close();

		/**
		 * Resets all primary buffers created from this pool.
		 *
		 * @param release whether to free all of the resources from the command pool back to the system
		 * @see https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkResetCommandPool.html
		 */
		void reset(bool release) const;

		/**
		 * Allocates a new CommandBuffer that can be used for recording,
		 * only one thread can use CommandBuffer allocated from one CommandPool, use multiple pools.
		 *
		 * @param level primary (directly send to the queue) or secondary (attached to another buffer)
		 */
		CommandBuffer allocate(VkCommandBufferLevel level) const;

		/**
		 * Create new vulkan CommandPool, command pools are used to create CommandBuffers
		 * that are needed to record and execute GPU commands
		 *
		 * @param family    The queue family this command pools will serve
		 * @param transient Specifies whether the allocated Command Buffers will be transient or not
		 */
		static CommandPool create(LogicalDevice& device, const Family& family, bool transient);

};

