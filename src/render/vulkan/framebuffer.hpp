#pragma once

#include "external.hpp"
#include "device.hpp"
#include "attachment.hpp"
#include "swapchain.hpp"

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

		std::vector<Framebuffer> framebuffers;
		std::vector<std::reference_wrapper<const Attachment>> attachments;

	public:

		FramebufferSet() = default;
		FramebufferSet(FramebufferSet& self) = default;

		void close();

		FramebufferSet(const std::vector<Framebuffer>&& framebuffers);
		void addAttachment(const Attachment& attachment);
		void construct(VkRenderPass vk_pass, VkDevice vk_device, const Swapchain& swapchain);

};