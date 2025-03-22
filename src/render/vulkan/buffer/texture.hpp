#pragma once

#include "external.hpp"
#include "image.hpp"
#include "render/vulkan/setup/device.hpp"

class Attachment;

class Texture {

	private:

		Image image;
		VkImageView vk_view = VK_NULL_HANDLE;
		VkSampler vk_sampler = VK_NULL_HANDLE;

	public:

		Texture() = default;
		Texture(const Image& image, VkImageView vk_view, VkSampler vk_sampler);

		void closeImageViewSampler(VkDevice vk_device);
		void closeViewSampler(VkDevice vk_device);

		VkFormat getFormat() const;
		VkImage getImage() const;
		VkImageView getView() const;
		VkSampler getSampler() const;

		/// Get the underlying Image object
		Image getTextureImage() const;

};

class TextureDelegate {

	private:

		std::string debug_name;
		VkImageUsageFlags vk_usage {};
		VkClearValue vk_clear {};
		VkImageViewCreateInfo view_info {};
		VkSamplerCreateInfo sampler_info {};
		VkSampleCountFlagBits vk_samples;

		friend class TextureBuilder;
		friend class Attachment;

		void checkImageFormat(VkFormat provided) const;

	public:

		TextureDelegate() = default;
		TextureDelegate(VkImageUsageFlags usage, VkClearValue clear, VkImageViewCreateInfo view, VkSamplerCreateInfo sampler, VkSampleCountFlagBits samples, const std::string& debug_name);

		Texture buildTexture(LogicalDevice& device, const Image& image) const;
		Attachment buildAttachment();

};

class TextureBuilder {

	private:

		std::string debug_name;
		VkImageUsageFlags vk_usage {};
		VkClearValue vk_clear {};
		VkImageViewCreateInfo view_info {};
		VkSamplerCreateInfo sampler_info {};
		VkSampleCountFlagBits vk_samples = VK_SAMPLE_COUNT_1_BIT;

	public:

		static TextureBuilder begin();

	public:

		TextureBuilder();

		/// Set texture type (1D, 2D, 3D, etc.)
		TextureBuilder& setType(VkImageViewType type);

		/// Rebind color channels (eg. .r=b, .g=1, .b=g)
		TextureBuilder& setSwizzle(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a);

		/// Type of data viewed from the image (COLOR, DEPTH, STENCIL, etc.)
		TextureBuilder& setAspect(VkImageAspectFlags aspect);

		/// Filtering method (NEAREST, LINEAR, etc.)
		TextureBuilder& setFilter(VkFilter filter);

		/// Addressing method outside valid UV bounds (repeat, clamp to edge, border, etc.)
		TextureBuilder& setAddressing(VkSamplerAddressMode mode);

		/// @see https://en.wikipedia.org/wiki/Anisotropy
		TextureBuilder& setAnisotropy(float anisotropy);

		/// Set the texture's debug name, visible in renderdoc
		TextureBuilder& setDebugName(const std::string& name);

		/// Set border color (ignored is addressing is not 'border')
		TextureBuilder& setBorder(VkBorderColor border);

		/// Relevant for attachments only, set clear color float value
		TextureBuilder& setClearColor(float r, float g, float b, float a);

		/// Relevant for attachments only, set clear color int value
		TextureBuilder& setClearColor(int r, int g, int b, int a);

		/// Relevant for attachments only, set depth color value
		TextureBuilder& setClearDepth(float depth, uint32_t stencil = 0);

		/// Relevant for attachments only, set the expected image format
		TextureBuilder& setFormat(VkFormat format);

		/// Relevant for attachments only, set the per-pixel sample count
		TextureBuilder& setSampleCount(VkSampleCountFlagBits samples);

		/// Relevant for attachments only, set attachment image usage
		TextureBuilder& setUsage(VkImageUsageFlags usage);

	public:

		/// Create a deferred, preconfigured texture/attachment template
		TextureDelegate createDelegate() const;

		/// Create a texture given an image
		Texture createTexture(LogicalDevice& device, const Image& image) const;

		/// Create an attachment, that is a full-screen texture that CAN be used in render passes
		Attachment createAttachment() const;

};
