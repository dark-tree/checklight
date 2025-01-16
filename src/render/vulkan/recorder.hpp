#pragma once

#include "external.hpp"

class Buffer;
class Image;

class CommandRecorder {

	private:

		VkCommandBuffer vk_buffer;

	public:

		CommandRecorder(VkCommandBuffer vk_buffer);

		/// Buffer recording must always end with a done() call
		void done();

		/// Copy data between two buffers
		CommandRecorder& copyBufferToBuffer(Buffer dst, Buffer src, size_t size);

		/// Copy data from buffer to image
		CommandRecorder& copyBufferToImage(Image dst, Buffer src, size_t offset, size_t width, size_t height, size_t layers, size_t level);

		/// Insert pipeline layout barrier
		CommandRecorder& transitionLayout(Image image, VkImageLayout dst, VkImageLayout src, size_t layers, size_t levels);

		/// Insert pipeline barrier that waits for the given stage
		CommandRecorder& bufferTransferBarrier(VkPipelineStageFlags dst);

};
