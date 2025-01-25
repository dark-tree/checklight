
#include "recorder.hpp"
#include "render/vulkan/buffer/image.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/vulkan/pass/render.hpp"
#include "render/vulkan/pass/pipeline.hpp"
#include "render/vulkan/descriptor/descriptor.hpp"

CommandRecorder::CommandRecorder(VkCommandBuffer vk_buffer)
: vk_buffer(vk_buffer) {}

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
	vkCmdBindPipeline(vk_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getHandle());
	return *this;
}

CommandRecorder& CommandRecorder::bindDescriptorSet(DescriptorSet& set) {
	VkDescriptorSet vk_set = set.getHandle();
	vkCmdBindDescriptorSets(vk_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_layout, 0, 1, &vk_set, 0, nullptr);
	return *this;
}

CommandRecorder& CommandRecorder::bindVertexBuffer(const Buffer& buffer, VkDeviceSize offset) {
	VkDeviceSize offsets[] = {offset};
	VkBuffer buf = buffer.getHandle();
	vkCmdBindVertexBuffers(vk_buffer, 0, 1, &buf, offsets);
	return *this;
}

CommandRecorder& CommandRecorder::bindIndexBuffer(const Buffer& buffer, VkDeviceSize offset) {
	vkCmdBindIndexBuffer(vk_buffer, buffer.getHandle(), offset, VK_INDEX_TYPE_UINT32);
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

CommandRecorder& CommandRecorder::copyBufferToBuffer(Buffer dst, Buffer src, size_t size) {
	VkBufferCopy region {};
	region.size = size;

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

CommandRecorder& CommandRecorder::transitionLayout(Image image, VkImageLayout dst, VkImageLayout src, size_t layers, size_t levels) {
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
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = levels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layers;

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
		dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
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

CommandRecorder& CommandRecorder::bufferTransferBarrier(VkPipelineStageFlags dst) {
	VkMemoryBarrier barrier {};
	barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

	VkPipelineStageFlags src = VK_PIPELINE_STAGE_TRANSFER_BIT;

	// allow reading from already written to sections (the whole thing doesn't need to finish)
	VkDependencyFlags flags = VK_DEPENDENCY_BY_REGION_BIT;

	vkCmdPipelineBarrier(vk_buffer, src, dst, flags, 1, &barrier, 0, nullptr, 0, nullptr);
	return *this;
}