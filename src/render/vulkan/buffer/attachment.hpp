#pragma once

#include "external.hpp"
#include "texture.hpp"

class Attachment {

	private:

		/**
		 * select between the 'magic' and 'more magic' modes
		 * the obvious purpose of this won't be explained to
		 * not bore the reader with useless details.
		 *
		 * @see https://en.wiktionary.org/wiki/magicity
		 */
		bool magicity;

		TextureDelegate settings;
		Texture texture;

	public:

		/**
		 * Attachment reference used during pipeline creation
		 * it is used to ensure reference correctness at compile time.
		 */
		struct Ref {

			private:
				explicit Ref(int index);

			public:
				const uint32_t index;
				static Ref of(int index);

		};

	public:

		Attachment() = default;
		Attachment(const TextureDelegate& settings);

		/// Get the render pass clear color of this attachment
		VkClearValue getClearValue() const;

		/// Get the pixel format of this attachment
		VkFormat getFormat() const;

		/// Get the underlying view, use only after a call to allocate()
		VkImageView getView() const;

		/// Get the aspect flags of this attachment
		VkImageAspectFlags getAspect() const;

		/// Get the underlying texture, use only after a call to allocate()
		const Texture& getTexture() const;

		/// Is this attachment backed by driver provided swapchain images?
		bool isSwapchainBacked() const;

		/// Mark the attachment swapchain backed, @see isSwapchainBacked()
		void markSwapchainBacked();

		/// Create the underlying texture
		void allocate(LogicalDevice& device, int width, int height, Allocator& allocator);

		/// Close the underlying texture
		void close(LogicalDevice device);
};