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

		VkClearValue getClearValue() const;
		VkFormat getFormat() const;
		VkImageView getView() const;
		const Texture& getTexture() const;

		bool isSwapchainBacked() const;
		void markSwapchainBacked();

		void allocate(LogicalDevice& device, int width, int height, Allocator& allocator);

		void close(LogicalDevice device);
};