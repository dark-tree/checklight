#pragma once

#include "external.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "render/vulkan/buffer/attachment.hpp"

class Framebuffer {

	private:

		VkFramebuffer vk_buffer;
		VkDevice vk_device;
		uint32_t presentation_index;
		std::vector<VkImageView> owned;

	public:

		Framebuffer() = default;
		Framebuffer(VkFramebuffer vk_buffer, VkDevice vk_device, uint32_t presentation_index);

		void close();

		VkFramebuffer getHandle() const;
};

class FramebufferBuilder {

	private:

		VkRenderPass pass;
		uint32_t width, height;
		std::vector<VkImageView> attachments;

	public:

		FramebufferBuilder(VkRenderPass vk_pass, VkExtent2D extent);

		void addAttachment(VkImageView vk_view);

		Framebuffer build(VkDevice vk_device, uint32_t presentation_index = 0);

};

class FramebufferSet {

	private:

		std::string debug_name;
		std::vector<Framebuffer> framebuffers;
		std::vector<std::reference_wrapper<const Attachment>> attachments;

	public:

		FramebufferSet() = default;
		FramebufferSet(FramebufferSet& self) = default;

		void setDebugName(const std::string& name);
		void close();

		FramebufferSet(const std::vector<Framebuffer>&& framebuffers);
		void addAttachment(const Attachment& attachment);
		void construct(VkRenderPass vk_pass, VkDevice vk_device, const Swapchain& swapchain);
		Framebuffer& byPresentationIndex(uint32_t index);

};