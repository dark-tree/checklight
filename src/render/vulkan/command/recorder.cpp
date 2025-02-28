
#include "recorder.hpp"
#include "render/vulkan/buffer/image.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/vulkan/pass/render.hpp"
#include "render/vulkan/pass/pipeline.hpp"
#include "render/vulkan/descriptor/descriptor.hpp"
#include "render/api/vertex.hpp"
#include "render/vulkan/setup/proxy.hpp"
#include "render/vulkan/raytrace/config.hpp"
#include "render/vulkan/buffer/query.hpp"
#include "submitter.hpp"

/*
 * CommandRecorder
 */

CommandRecorder::CommandRecorder(VkCommandBuffer vk_buffer, VkCommandBufferUsageFlags flags)
	: vk_buffer(vk_buffer), flags(flags) {

	vkResetCommandBuffer(vk_buffer, 0);

	VkCommandBufferBeginInfo info {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.flags = flags;
	info.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(vk_buffer, &info) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to begin recording a command buffer!"};
	}

}

void CommandRecorder::done() {
	if (vkEndCommandBuffer(vk_buffer) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to record a command buffer!"};
	}
}

CommandRecorder& CommandRecorder::beginRenderPass(RenderPass& render_pass, uint32_t image, VkExtent2D extent) {

	tracer.reset(render_pass);

	VkRenderPassBeginInfo info {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.renderPass = render_pass.vk_pass;
	info.framebuffer = render_pass.getFramebuffer(image);

	info.renderArea.offset = {0, 0};
	info.renderArea.extent = extent;

	// attachments define their own clear values
	const std::vector<VkClearValue>& values = render_pass.values;
	info.clearValueCount = values.size();
	info.pClearValues = values.data();

	vkCmdBeginRenderPass(vk_buffer, &info, VK_SUBPASS_CONTENTS_INLINE);
	return *this;
}

CommandRecorder& CommandRecorder::nextSubpass() {
	tracer.advance();
	vkCmdNextSubpass(vk_buffer, VK_SUBPASS_CONTENTS_INLINE);
	return *this;
}

CommandRecorder& CommandRecorder::bindPipeline(GraphicsPipeline& pipeline) {
	this->vk_layout = pipeline.getLayout();
	this->vk_bind = pipeline.getBindPoint();
	vkCmdBindPipeline(vk_buffer, vk_bind, pipeline.getHandle());
	return *this;
}

CommandRecorder& CommandRecorder::bindDescriptorSet(DescriptorSet& set) {
	VkDescriptorSet vk_set = set.getHandle();
	vkCmdBindDescriptorSets(vk_buffer, vk_bind, vk_layout, 0, 1, &vk_set, 0, nullptr);
	return *this;
}

CommandRecorder& CommandRecorder::bindVertexBuffer(const Buffer& buffer, VkDeviceSize offset) {
	VkDeviceSize offsets[] = {offset};
	VkBuffer buf = buffer.getHandle();
	vkCmdBindVertexBuffers(vk_buffer, 0, 1, &buf, offsets);
	return *this;
}

CommandRecorder& CommandRecorder::bindIndexBuffer(const Buffer& buffer, VkDeviceSize offset) {
	vkCmdBindIndexBuffer(vk_buffer, buffer.getHandle(), offset, Index3D::format);
	return *this;
}

CommandRecorder& CommandRecorder::draw(uint32_t vertices, uint32_t instances, uint32_t vertexIndexOffset, uint32_t instanceIndexOffset) {
	vkCmdDraw(vk_buffer, vertices, instances, vertexIndexOffset, instanceIndexOffset);
	return *this;
}

CommandRecorder& CommandRecorder::drawIndexed(uint32_t indexes, uint32_t instances, uint32_t firstIndex, int32_t vertexOffset, uint32_t instanceIndexOffset) {
	vkCmdDrawIndexed(vk_buffer, indexes, instances, firstIndex, vertexOffset, instanceIndexOffset);
	return *this;
}

CommandRecorder& CommandRecorder::endRenderPass() {
	tracer.end();
	vkCmdEndRenderPass(vk_buffer);
	return *this;
}

CommandRecorder& CommandRecorder::copyBufferToBuffer(Buffer dst, Buffer src, size_t size, size_t dst_offset, size_t src_offset) {
	VkBufferCopy region {};
	region.size = size;
//	region.dstOffset = dst_offset;
//	region.srcOffset = src_offset;

	vkCmdCopyBuffer(vk_buffer, src.getHandle(), dst.getHandle(), 1, &region);
	return *this;
}

CommandRecorder& CommandRecorder::copyBufferToImage(Image dst, Buffer src, size_t offset, size_t width, size_t height, size_t layers, size_t level) {
	VkBufferImageCopy region {};
	region.bufferOffset = offset;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = level;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = layers;

	region.imageOffset = {0, 0, 0};
	region.imageExtent.width = width;
	region.imageExtent.height = height;
	region.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(vk_buffer, src.getHandle(), dst.getHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	return *this;
}

CommandRecorder& CommandRecorder::updateBuffer(Buffer buffer, void* data) {
	vkCmdUpdateBuffer(vk_buffer, buffer.getHandle(), 0, buffer.size(), data);
	return *this;
}

CommandRecorder& CommandRecorder::transitionLayout(Attachment& attachment, VkImageLayout dst, VkImageLayout src) {
	return transitionLayout(attachment.getTexture().getTextureImage(), dst, src, attachment.getAspect());
}

CommandRecorder& CommandRecorder::transitionLayout(Image image, VkImageLayout dst, VkImageLayout src, VkImageAspectFlags aspect) {
	VkPipelineStageFlags src_stage = 0;
	VkPipelineStageFlags dst_stage = 0;

	bool failed = true;

	VkImageMemoryBarrier barrier {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = src;
	barrier.newLayout = dst;

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image.getHandle();
	barrier.subresourceRange.aspectMask = aspect;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = image.getLayerCount();
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = image.getLayerCount();

	if (src == VK_IMAGE_LAYOUT_UNDEFINED && dst == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		failed = false;
	}

	if (src == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && dst == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
		failed = false;
	}

	if (src == VK_IMAGE_LAYOUT_UNDEFINED && dst == VK_IMAGE_LAYOUT_GENERAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

		src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
		failed = false;
	}

	if (src == VK_IMAGE_LAYOUT_GENERAL && dst == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
		dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
		failed = false;
	}

	// allow reading from already written to sections (the whole thing doesn't need to finish)
	VkDependencyFlags flags = VK_DEPENDENCY_BY_REGION_BIT;

	if (failed) {
		throw std::runtime_error {"Unknown layout transition!"};
	}

	vkCmdPipelineBarrier(vk_buffer, src_stage, dst_stage, flags, 0, nullptr, 0, nullptr, 1, &barrier);
	return *this;
}

MemoryBarrier CommandRecorder::memoryBarrier() {
	return MemoryBarrier::create(vk_buffer);
}

CommandRecorder& CommandRecorder::bufferTransferBarrier(VkPipelineStageFlags stage) {
	memoryBarrier().first(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT).then(stage, VK_ACCESS_MEMORY_READ_BIT).byRegionBit().done();
	return *this;
}

CommandRecorder& CommandRecorder::writePushConstant(const PushConstant& constant, const void* data) {
	VkPushConstantRange range = constant.getRange();
	vkCmdPushConstants(vk_buffer, vk_layout, range.stageFlags, range.offset, range.size, data);
	return *this;
}

CommandRecorder& CommandRecorder::buildAccelerationStructure(const AccelStructBakedConfig& config) {
	if (!config.ready) {
		throw std::runtime_error {"Unable to build unfinalized baked config!"};
	}

	const auto* buffer = config.ranges.data();
	Proxy::vkCmdBuildAccelerationStructuresKHR(vk_buffer, 1, &config.build_info, &buffer);
	return *this;
}

CommandRecorder& CommandRecorder::copyAccelerationStructure(AccelStruct& dst, AccelStruct& src, bool compact) {
	VkCopyAccelerationStructureInfoKHR copy_info {};
	copy_info.sType = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
	copy_info.pNext = nullptr;
	copy_info.dst = dst.getHandle();
	copy_info.src = src.getHandle();
	copy_info.mode = (compact ? VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR : VK_COPY_ACCELERATION_STRUCTURE_MODE_CLONE_KHR);
	Proxy::vkCmdCopyAccelerationStructureKHR(vk_buffer, &copy_info);
	return *this;
}

CommandRecorder& CommandRecorder::queryAccelStructProperties(QueryPool& pool, const std::vector<VkAccelerationStructureKHR>& structures, VkQueryType type) {
	Proxy::vkCmdWriteAccelerationStructuresPropertiesKHR(vk_buffer, structures.size(), structures.data(), type, pool.getHandle(), 0);
	return *this;
}

CommandRecorder& CommandRecorder::quickFenceSubmit(Fence& fence, Queue& queue) {
	done();
	CommandSubmitter {vk_buffer} .signal(fence).done(queue);
	fence.wait();
	CommandRecorder {vk_buffer, flags};
	return *this;
}

CommandRecorder& CommandRecorder::resetQueryPool(QueryPool& pool) {
	vkCmdResetQueryPool(vk_buffer, pool.getHandle(), 0, pool.size());
	return *this;
}

CommandRecorder& CommandRecorder::traceRays(ShaderTable& shaders, int width, int height) {
	Proxy::vkCmdTraceRaysKHR(vk_buffer, &shaders.vk_region_generate, &shaders.vk_region_miss, &shaders.vk_region_hit, &shaders.vk_region_call, width, height, 1);
	return *this;
}

CommandRecorder& CommandRecorder::blit(const Image& dst, VkImageLayout layout_dst, const Image& src, VkImageLayout layout_src) {

	VkImageSubresourceLayers src_subresource {};
	src_subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	src_subresource.layerCount = src.getLayerCount();
	src_subresource.baseArrayLayer = 0;
	src_subresource.mipLevel = 0;

	VkImageSubresourceLayers dst_subresource {};
	dst_subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	dst_subresource.layerCount = dst.getLayerCount();
	dst_subresource.baseArrayLayer = 0;
	dst_subresource.mipLevel = 0;

	VkOffset3D src_offset[2];
	src_offset[0] = {0, 0, 0};
	src_offset[1] = {0, 0, 0};

	VkOffset3D dst_offset[2];
	dst_offset[0] = {0, 0, 0};
	dst_offset[1] = {0, 0, 0};

	VkImageBlit blit {};
	blit.dstSubresource = dst_subresource;
	memcpy(blit.dstOffsets, dst_offset, sizeof(VkOffset3D) * 2);
	blit.srcSubresource = src_subresource;
	memcpy(blit.srcOffsets, src_offset, sizeof(VkOffset3D) * 2);

	vkCmdBlitImage(vk_buffer, src.getHandle(), layout_src, dst.getHandle(), layout_dst, 1, &blit, VK_FILTER_NEAREST);
	return *this;
}
