#pragma once

#include "external.hpp"
#include "texture.hpp"

class Attachment {

	private:

		TextureDelegate settings;
		Texture texture;

	public:

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

		void allocate(LogicalDevice& device, int width, int height, Allocator& allocator);

};