#pragma once

#include "external.hpp"
#include "render/vulkan/pass/tracer.hpp"
#include "render/vulkan/descriptor/push.hpp"
#include "barrier.hpp"

struct AccelStructBakedConfig;
class Buffer;
class Image;
class GraphicsPipeline;
class DescriptorSet;
class AccelStruct;
class QueryPool;
class Queue;
class Fence;
class ShaderTable;
class Attachment;

class CommandRecorder {

	private:

		VkCommandBuffer vk_buffer;
		VkPipelineLayout vk_layout;
		VkPipelineBindPoint vk_bind;
		RenderPassTracer tracer;
		VkCommandBufferUsageFlags flags;

	public:

		CommandRecorder() = default;
		CommandRecorder(VkCommandBuffer vk_buffer, VkCommandBufferUsageFlags flags);

		/// Buffer recording must always end with a done() call
		void done();

		/// Copy data between two buffers
		CommandRecorder& copyBufferToBuffer(Buffer dst, Buffer src, size_t size, size_t dst_offset, size_t src_offset);

		/// Copy data from buffer to image
		CommandRecorder& copyBufferToImage(Image dst, Buffer src, size_t offset, size_t width, size_t height, size_t layers, size_t level);

		/// Update buffer from host memory
		CommandRecorder& updateBuffer(Buffer buffer, void* data);

		/// Insert pipeline layout barrier
		CommandRecorder& transitionLayout(Attachment& attachment, VkImageLayout dst, VkImageLayout src);

		/// Insert pipeline layout barrier
		CommandRecorder& transitionLayout(Image image, VkImageLayout dst, VkImageLayout src, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

		/// Begin a simple to use memory barrier builder
		MemoryBarrier memoryBarrier();

		/// Insert pipeline barrier that waits for a WRITE before a READ access
		CommandRecorder& bufferTransferBarrier(VkPipelineStageFlags dst);

		/// Begins the first subpass in the given render pass
		CommandRecorder& beginRenderPass(RenderPass& render_pass, uint32_t image, VkExtent2D extent);

		/// Advances to the next subpass in the currently bound render pass
		CommandRecorder& nextSubpass();

		/// Binds a pipline, the pipeline must be compatible with the current subpass
		CommandRecorder& bindPipeline(GraphicsPipeline& pipeline);

		/// Bind a descriptor set. Descriptor sets can be bound independently of pipelines
		CommandRecorder& bindDescriptorSet(DescriptorSet& set);

		/// Bind a vertex buffer
		CommandRecorder& bindVertexBuffer(const Buffer& buffer, VkDeviceSize offset = 0);

		/// Bind a vertex buffer
		CommandRecorder& bindIndexBuffer(const Buffer& buffer, VkDeviceSize offset = 0);

		/// Issue a draw command, requires a pipeline and subpass to be bound at the same time
		CommandRecorder& draw(uint32_t vertices, uint32_t instances = 1, uint32_t vertexIndexOffset = 0, uint32_t instanceIndexOffset = 0);

		/// Issue a indexed draw command,  requires a pipeline and subpass to be bound at the same time
		CommandRecorder& drawIndexed(uint32_t indexes, uint32_t instances = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t instanceIndexOffset = 0);

		/// Ends the render pass, requires the render pass to be bound the the subpass index to point to the last subpass in that renderpass
		CommandRecorder& endRenderPass();

		/// Writes data to the pipeline's push constant, the data is immediately available to the subsequent draw calls
		CommandRecorder& writePushConstant(const PushConstant& constant, const void* data);

		/// Builds or updates an acceleration structure given a preconfigured struct
		CommandRecorder& buildAccelerationStructure(const AccelStructBakedConfig& config);

		/// Copy data from one acceleration structure to another, potentially compacting the data during the operation
		CommandRecorder& copyAccelerationStructure(AccelStruct& dst, AccelStruct& src, bool compact);

		/// Used to get some post-creation detail about a acceleration structure
		CommandRecorder& queryAccelStructProperties(QueryPool& pool, const std::vector<VkAccelerationStructureKHR>& structures, VkQueryType type);

		/// Please don't use this command :skull:
		CommandRecorder& quickFenceSubmit(Fence& fence, Queue& queue);

		/// Resets the given query pool, pools need to be reset after creation and between uses
		CommandRecorder& resetQueryPool(QueryPool& pool);

		/// Invokes the raytracing machinery using the provided shader table
		CommandRecorder& traceRays(ShaderTable& shaders, int width, int height);

		/// Blit one image into another
		CommandRecorder& blit(const Image& dst, VkImageLayout layout_dst, const Image& src, VkImageLayout layout_src);

		/// Clear the given attachment
		CommandRecorder& clearAttachment(const Attachment& attachment);
};
