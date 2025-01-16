
#include "attachment.hpp"

Attachment::Attachment(const TextureDelegate& settings)
: settings(settings), texture() {
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

void Attachment::allocate(LogicalDevice& device, int width, int height, Allocator& allocator) {
	Image image = allocator.allocateImage(Memory::DEVICE, width, height, getFormat(), settings.vk_usage, 1, 1);
	texture = settings.buildTexture(device, image);
}