#pragma once

#include "external.hpp"
#include "image.hpp"
#include "device.hpp"

class Texture {

	private:

		VkImage vk_image;
		VkImageView vk_view;
		VkSampler vk_sampler;

	public:

		Texture(VkImage vk_image, VkImageView vk_view, VkSampler vk_sampler)
		: vk_image(vk_image), vk_view(vk_view), vk_sampler(vk_sampler) {}

};

class TextureBuilder {

	private:

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
