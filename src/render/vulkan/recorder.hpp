#pragma once

#include "external.hpp"
#include "pass/tracer.hpp"

class Buffer;
class Image;
class GraphicsPipeline;
class DescriptorSet;

class CommandRecorder {

	private:

		VkCommandBuffer vk_buffer;
		VkPipelineLayout vk_layout;
		RenderPassTracer tracer;

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

		CommandRecorder& beginRenderPass(RenderPass& render_pass, uint32_t image, VkExtent2D extent);

		CommandRecorder& nextSubpass();

		CommandRecorder& bindPipeline(GraphicsPipeline& pipeline);

		CommandRecorder& bindDescriptorSet(DescriptorSet& set);

		CommandRecorder& bindVertexBuffer(const Buffer& buffer, VkDeviceSize offset = 0);

		CommandRecorder& bindIndexBuffer(const Buffer& buffer, VkDeviceSize offset = 0);

		CommandRecorder& draw(uint32_t vertices, uint32_t instances = 1, uint32_t vertexIndexOffset = 0, uint32_t instanceIndexOffset = 0);

		CommandRecorder& drawIndexed(uint32_t indexes, uint32_t instances = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t instanceIndexOffset = 0);

		CommandRecorder& endRenderPass();
};
