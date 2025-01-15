
#include "texture.hpp"
#include "attachment.hpp"

/*
 * Texture
 */

Texture::Texture(VkFormat vk_format, VkImage vk_image, VkImageView vk_view, VkSampler vk_sampler)
: vk_format(vk_format), vk_image(vk_image), vk_view(vk_view), vk_sampler(vk_sampler) {}

Attachment Texture::asColorAttachment(float r, float g, float b, float a) {
	VkClearValue value;
	value.color = {.float32 = {r, g, b, a}};
	return {*this, value};
}

Attachment Texture::asColorAttachment(int r, int g, int b, int a) {
	VkClearValue value;
	value.color = {.int32 = {r, g, b, a}};
	return {*this, value};
}

Attachment Texture::asDepthAttachment(float depth, uint32_t stencil) {
	VkClearValue value;
	value.depthStencil = {depth, stencil};
	return {*this, value};
}

/*
 * TextureBuilder
 */

TextureBuilder TextureBuilder::create(const Image image) {
	return {image};
}

TextureBuilder::TextureBuilder(const Image image) {
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = image.getHandle();

	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = image.getLayerCount();
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = image.getLevelCount();

	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

	vk_format = image.getFormat();

	setType(VK_IMAGE_VIEW_TYPE_2D);
	setSwizzle(VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY);
	setFilter(VK_FILTER_LINEAR);
	setAddressing(VK_SAMPLER_ADDRESS_MODE_REPEAT);
	setAnisotropy(0.0f);
	setBorder(VK_BORDER_COLOR_INT_OPAQUE_BLACK);
	setAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT);
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

TextureBuilder& TextureBuilder::setAspectFlags(VkImageAspectFlags aspect) {
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

Texture TextureBuilder::build(LogicalDevice& device) const {

	VkImage image = view_info.image;
	VkImageView view;
	VkSampler sampler;

	if (vkCreateImageView(device.getHandle(), &view_info, nullptr, &view) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create image view!"};
	}

	if (vkCreateSampler(device.getHandle(), &sampler_info, nullptr, &sampler) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create image sampler!"};
	}

	return {vk_format, image, view, sampler};

}