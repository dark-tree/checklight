
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
		throw std::runtime_error {"Attachment format can't be left undefined!"};
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

const Texture& Attachment::getTexture() const {
	return texture;
}

bool Attachment::isSwapchainBacked() const {
	return magicity;
}

void Attachment::markSwapchainBacked(){
	magicity = true;
}

void Attachment::allocate(LogicalDevice& device, int width, int height, Allocator& allocator) {
	Image image = allocator.allocateImage(Memory::DEVICE, width, height, getFormat(), settings.vk_usage, 1, 1, settings.debug_name.c_str());
	texture = settings.buildTexture(device, image);
}

void Attachment::close(LogicalDevice device) {
	texture.closeImageViewSampler(device.getHandle());
}
