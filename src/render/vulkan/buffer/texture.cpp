
#include "texture.hpp"
#include "attachment.hpp"

/*
 * Texture
 */

Texture::Texture(VkFormat vk_format, VkImage vk_image, VkImageView vk_view, VkSampler vk_sampler)
: vk_format(vk_format), vk_image(vk_image), vk_view(vk_view), vk_sampler(vk_sampler) {}

VkFormat Texture::getFormat() const {
	return vk_format;
}

VkImage Texture::getImage() const {
	return vk_image;
}

VkImageView Texture::getView() const {
	return vk_view;
}

VkSampler Texture::getSampler() const {
	return vk_sampler;
}

/*
 * TextureDelegate
 */

void TextureDelegate::checkImageFormat(VkFormat provided) const {
	if (view_info.format == VK_FORMAT_UNDEFINED) {
		return;
	}

	if (view_info.format != provided) {
		throw std::runtime_error {"Image format incompatible, was the contain set intentionally?"};
	}
}

TextureDelegate::TextureDelegate(VkImageUsageFlags usage, VkClearValue clear, VkImageViewCreateInfo view, VkSamplerCreateInfo sampler)
: vk_usage(usage), vk_clear(clear), view_info(view), sampler_info(sampler) {}

Texture TextureDelegate::buildTexture(LogicalDevice& device, const Image& image) const {

	VkImageViewCreateInfo view_info = this->view_info;
	checkImageFormat(image.getFormat());

	// complete view create info creation
	view_info.image = image.getHandle();
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = image.getLayerCount();
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = image.getLevelCount();
	view_info.format = image.getFormat();

	VkImageView view;
	VkSampler sampler;

	if (vkCreateImageView(device.getHandle(), &view_info, nullptr, &view) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create image view!"};
	}

	if (vkCreateSampler(device.getHandle(), &sampler_info, nullptr, &sampler) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create image sampler!"};
	}

	return {image.getFormat(), image.getHandle(), view, sampler};

}

Attachment TextureDelegate::buildAttachment() {
	return {*this};
}

/*
 * TextureBuilder
 */

TextureBuilder TextureBuilder::begin() {
	return {};
}

TextureBuilder::TextureBuilder() {
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

	setType(VK_IMAGE_VIEW_TYPE_2D);
	setSwizzle(VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY);
	setFilter(VK_FILTER_LINEAR);
	setAddressing(VK_SAMPLER_ADDRESS_MODE_REPEAT);
	setAnisotropy(0.0f);
	setBorder(VK_BORDER_COLOR_INT_OPAQUE_BLACK);
	setAspect(VK_IMAGE_ASPECT_COLOR_BIT);
	setFormat(VK_FORMAT_UNDEFINED);
	setUsage(0);
}

TextureBuilder& TextureBuilder::setType(VkImageViewType type) {
	view_info.viewType = type;
	return *this;
}

TextureBuilder& TextureBuilder::setSwizzle(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a) {
	view_info.components.r = r;
	view_info.components.g = g;
	view_info.components.b = b;
	view_info.components.a = a;
	return *this;
}

TextureBuilder& TextureBuilder::setAspect(VkImageAspectFlags aspect) {
	view_info.subresourceRange.aspectMask = aspect;
	return *this;
}

TextureBuilder& TextureBuilder::setFilter(VkFilter filter) {
	sampler_info.magFilter = filter;
	sampler_info.minFilter = filter;
	return *this;
}

TextureBuilder& TextureBuilder::setAddressing(VkSamplerAddressMode mode) {
	sampler_info.addressModeU = mode;
	sampler_info.addressModeV = mode;
	sampler_info.addressModeW = mode;
	return *this;
}

TextureBuilder& TextureBuilder::setAnisotropy(float anisotropy) {
	sampler_info.anisotropyEnable = (anisotropy > 0);
	sampler_info.maxAnisotropy = anisotropy;
	return *this;
}

TextureBuilder& TextureBuilder::setBorder(VkBorderColor border) {
	sampler_info.borderColor = border;
	return *this;
}

TextureBuilder& TextureBuilder::setClearColor(float r, float g, float b, float a) {
	vk_clear.color = {.float32 = {r, g, b, a}};
	return *this;
}

TextureBuilder& TextureBuilder::setClearColor(int r, int g, int b, int a) {
	vk_clear.color = {.int32 = {r, g, b, a}};
	return *this;
}

TextureBuilder& TextureBuilder::setClearDepth(float depth, uint32_t stencil) {
	vk_clear.depthStencil = {depth, stencil};
	return *this;
}

TextureBuilder& TextureBuilder::setFormat(VkFormat format) {
	view_info.format = format;
	return *this;
}

TextureBuilder& TextureBuilder::setUsage(VkImageUsageFlags usage) {
	vk_usage = usage;
	return *this;
}

TextureDelegate TextureBuilder::createDelegate() const {
	return {vk_usage, vk_clear, view_info, sampler_info};
}

Texture TextureBuilder::createTexture(LogicalDevice& device, const Image& image) const {
	return createDelegate().buildTexture(device, image);
}

Attachment TextureBuilder::createAttachment() const {
	return createDelegate().buildAttachment();
}