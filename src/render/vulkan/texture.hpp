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

		TextureBuilder(const Image image);

		/// Set texture type (1D, 2D, 3D, etc.)
		TextureBuilder& setType(VkImageViewType type);

		/// Rebind color channels (eg. .r=b, .g=1, .b=g)
		TextureBuilder& setSwizzle(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a);

		/// Type of data this will contain (COLOR, DEPTH, STENCIL, etc.)
		TextureBuilder& setAspectFlags(VkImageAspectFlags aspect);

		/// Filtering method (NEAREST, LINEAR, etc.)
		TextureBuilder& setFilter(VkFilter filter);

		/// Addressing method outside valid UV bounds (repeat, clamp to edge, border, etc.)
		TextureBuilder& setAddressing(VkSamplerAddressMode mode);

		/// @see https://en.wikipedia.org/wiki/Anisotropy
		TextureBuilder& setAnisotropy(float anisotropy);

		/// Set border color (ignored is addressing is not 'border')
		TextureBuilder& setBorder(VkBorderColor border);

		/// Create the Texture resource using given settigns
		Texture build(LogicalDevice& device) const;

};
