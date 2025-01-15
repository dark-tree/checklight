#pragma once

#include "external.hpp"

class Buffer;
class Image;

class CommandRecorder {

	private:

		VkCommandBuffer vk_buffer;

	public:

		CommandRecorder(VkCommandBuffer vk_buffer);

		/**
		 * TODO documentation
		 */
		void done();

		CommandRecorder& copyBufferToBuffer(Buffer dst, Buffer src, size_t size);
		CommandRecorder& copyBufferToImage(Image dst, Buffer src, size_t offset, size_t width, size_t height, size_t layers, size_t level);
		CommandRecorder& transitionLayout(Image image, VkImageLayout dst, VkImageLayout src, size_t layers, size_t levels);
		CommandRecorder& bufferTransferBarrier(VkPipelineStageFlags dst);

};
