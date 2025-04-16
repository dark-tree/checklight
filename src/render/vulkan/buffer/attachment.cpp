
#include "attachment.hpp"

/*
 * Attachment::Ref
 */

Attachment::Ref::Ref(int index)
: index(index) {}

Attachment::Ref Attachment::Ref::of(int index) {
	return Ref {index};
}

/*
 * Attachment
 */

Attachment::Attachment(const TextureDelegate& settings)
: magicity(false), settings(settings), texture() {
	this->settings.debug_name += " Attachment";

	if (settings.view_info.format == VK_FORMAT_UNDEFINED) {
		FAULT("Attachment format can't be left undefined!");
	}
}

VkClearValue Attachment::getClearValue() const {
	return settings.vk_clear;
}

VkFormat Attachment::getFormat() const {
	return settings.view_info.format;
}

VkImageView Attachment::getView() const {
	return texture.getView();
}

VkImageAspectFlags Attachment::getAspect() const {
	return settings.view_info.subresourceRange.aspectMask;
}

VkSampleCountFlagBits Attachment::getSamples() const {
	return settings.vk_samples;
}

const Texture& Attachment::getTexture() const {
	return texture;
}

bool Attachment::isSwapchainBacked() const {
	return magicity;
}

void Attachment::markSwapchainBacked(){
	magicity = true;
}

void Attachment::allocate(LogicalDevice& device, VkExtent2D extent, Allocator& allocator) {
	Image image = allocator.allocateImage(Memory::DEVICE, extent.width, extent.height, getFormat(), settings.vk_usage, 1, 1, settings.vk_samples, settings.debug_name.c_str());
	this->texture = settings.buildTexture(device, image);
	this->extent = extent;
}

void Attachment::close(LogicalDevice device) {
	texture.closeImageViewSampler(device.getHandle());
}

VkExtent2D Attachment::getExtent() const {
	return extent;
}
