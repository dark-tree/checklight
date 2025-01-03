#pragma once

#include "external.hpp"
#include "image.hpp"
#include "device.hpp"

class Attachment;

class Texture {

	private:

		VkFormat vk_format;
		VkImage vk_image;
		VkImageView vk_view;
		VkSampler vk_sampler;

	public:

		Texture(VkFormat vk_format, VkImage vk_image, VkImageView vk_view, VkSampler vk_sampler);

		/**
		 * Convert this texture to an Attachment with the given VkClearValue
		 */
		Attachment asColorAttachment(float r, float g, float b, float a);
		Attachment asColorAttachment(int r, int g, int b, int a);
		Attachment asDepthAttachment(float depth, uint32_t stencil = 0);

};

class TextureBuilder {

	private:

		VkFormat vk_format;
		VkImageViewCreateInfo view_info {};
		VkSamplerCreateInfo sampler_info {};

	public:

		static TextureBuilder create(const Image image);

	public:

		// TODO documentation

		TextureBuilder(const Image image);

		TextureBuilder& setType(VkImageViewType type);

		TextureBuilder& setSwizzle(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a);

		TextureBuilder& setAspectFlags(VkImageAspectFlags aspect);

		TextureBuilder& setFilter(VkFilter filter);

		TextureBuilder& setAddressing(VkSamplerAddressMode mode);

		TextureBuilder& setAnisotropy(float anisotropy);

		TextureBuilder& setBorder(VkBorderColor border);

		Texture build(LogicalDevice& device) const;

};
