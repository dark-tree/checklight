#pragma once

#include "external.hpp"
#include "subpass.hpp"
#include "dependency.hpp"
#include "attachment.hpp"
#include "render/vulkan/device.hpp"
#include "util/pyramid.hpp"
#include "render/vulkan/framebuffer.hpp"
#include "render/vulkan/swapchain.hpp"

class RenderPass {

	public:

		FramebufferSet framebuffer;
		VkDevice vk_device;
		VkRenderPass vk_pass;
		std::vector<VkClearValue> values;
		std::vector<Subpass> subpasses;

	public:

		RenderPass() = default;
		RenderPass(VkDevice vk_device, VkRenderPass vk_pass, std::vector<VkClearValue>& values, std::vector<Subpass>& subpass_info, FramebufferSet& framebuffer);

		void close();
		const Subpass& getSubpass(int subpass) const;
		int getSubpassCount() const;

		/**
		 * Allocate (or reallocate if already present) the underlying framebuffers
		 * to conform to the given parameters.
		 */
		void prepareFramebuffers(const Swapchain& swapchain);

		VkFramebuffer getFramebuffer(uint32_t i);
};

class RenderPassBuilder {

	private:

		FramebufferSet framebuffer;

		std::vector<VkClearValue> values;
		std::vector<AttachmentBuilder> attachments;
		std::vector<SubpassBuilder> subpasses;
		std::vector<DependencyBuilder> dependencies;

		Pyramid<uint32_t> preserve;

	public:

		// internal methods, don't use
		Attachment::Ref addAttachment(AttachmentBuilder& builder);
		RenderPassBuilder& addSubpass(SubpassBuilder& builder);
		RenderPassBuilder& addDependency(DependencyBuilder& builder);

	public:

		/**
		 * @brief Begin attachment creation
		 * Adds and makes usable an attachment for subpasses in this RenderPass
		 */
		AttachmentBuilder addAttachment(const Attachment& attachment, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

		/**
		 * @brief Begin subpass creation
		 * Adds a render pass sub-stage, subpasses are executed in order
		 */
		SubpassBuilder addSubpass(VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);

		/**
		 * @brief Begin dependency creation
		 * Add a data dependency between subpasses and stages
		 */
		DependencyBuilder addDependency(VkDependencyFlags flags = 0);

		/**
		 * Complete the building process and acquire the
		 * constructed RenderPass object.
		 */
		RenderPass build(const LogicalDevice& device);

};