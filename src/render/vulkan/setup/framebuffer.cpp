
#include "framebuffer.hpp"

/*
 * Framebuffer
 */

Framebuffer::Framebuffer(VkFramebuffer vk_buffer, VkDevice vk_device, uint32_t presentation_index)
: vk_buffer(vk_buffer), vk_device(vk_device), presentation_index(presentation_index) {}

void Framebuffer::close() {
	vkDestroyFramebuffer(vk_device, vk_buffer, nullptr);
}

VkFramebuffer Framebuffer::getHandle() const {
	return vk_buffer;
}

/*
 * FramebufferBuilder
 */

FramebufferBuilder::FramebufferBuilder(VkRenderPass vk_pass, VkExtent2D extent) {
	this->pass = vk_pass;
	this->width = extent.width;
	this->height = extent.height;
}

void FramebufferBuilder::addAttachment(VkImageView vk_view) {
	attachments.push_back(vk_view);
}

Framebuffer FramebufferBuilder::build(VkDevice vk_device, uint32_t presentation_index) {

	VkFramebufferCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	create_info.pNext = nullptr;
	create_info.renderPass = pass;

	create_info.attachmentCount = attachments.size();
	create_info.pAttachments = attachments.data();
	create_info.width = width;
	create_info.height = height;

	// TODO should this be passed by the image?
	create_info.layers = 1;
	create_info.flags = 0;

	VkFramebuffer framebuffer;

	if (vkCreateFramebuffer(vk_device, &create_info, nullptr, &framebuffer) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create a framebuffer!"};
	}

	return {framebuffer, vk_device, presentation_index};
}

/*
 * FramebufferSet
 */

FramebufferSet::FramebufferSet(const std::vector<Framebuffer>&& framebuffers)
: framebuffers(framebuffers) {};

void FramebufferSet::close() {
	for (Framebuffer& framebuffer : framebuffers) {
		framebuffer.close();
	}

	framebuffers.clear();
}

void FramebufferSet::addAttachment(const Attachment& attachment) {
	attachments.emplace_back(attachment);
}

void FramebufferSet::construct(VkRenderPass vk_pass, VkDevice vk_device, const Swapchain& swapchain) {

	close();

	std::vector<VkImageView> views;
	views.reserve(attachments.size());

	if (attachments.empty()) {
		throw std::runtime_error {"Can't create an empty framebuffer set!"};
	}

	std::vector<VkImageView> surfaces;
	int screen = -1;

	// extract vulkan image views
	for (uint32_t i = 0; i < attachments.size(); i ++) {
		const Attachment& attachment = attachments[i];
		VkImageView view = attachment.getView();

		if (attachment.isSwapchainBacked()) {

			// there can only be one swapchain-backed attachment per framebuffer
			if (screen != -1) {
				throw std::runtime_error {"Swapchain attachment already assigned to framebuffer at index: " + std::to_string(screen)};
			}

			// nothing to View here! (ha! did you get the pun?)
			views.push_back(VK_NULL_HANDLE);
			screen = i;

			continue;
		}

		if (view == VK_NULL_HANDLE) {
			throw std::runtime_error {"Null handle found during framebuffer creation at index: " + std::to_string(i)};
		}

		views.push_back(view);
	}

	if (screen != -1) {
		// copy handles if we need them
		for (const ImageView& view : swapchain.getViews()) {
			surfaces.push_back(view.getHandle());
		}
	} else {
		// otherwise cheat to use the same algorithm in both cases
		screen = 0;
		surfaces.push_back(views[0]);
		views[0] = VK_NULL_HANDLE;
	}

	framebuffers.reserve(surfaces.size());

	// create framebuffers
	for (int i = 0; i < (int) surfaces.size(); i ++) {
		FramebufferBuilder builder {vk_pass, swapchain.getExtend()};

		for (int j = 0; j < (int) views.size(); j ++) {
			VkImageView view = views[j];

			if (j == screen) {
				view = surfaces[i];
			}

			builder.addAttachment(view);
		}

		framebuffers.push_back(builder.build(vk_device, i));
	}
}

Framebuffer& FramebufferSet::byPresentationIndex(uint32_t index) {
	return framebuffers.at(index % framebuffers.size());
}