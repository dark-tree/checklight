#pragma once

#include "external.hpp"
#include "texture.hpp"

class Attachment {

	private:

		TextureDelegate settings;
		Texture texture;

	public:

		Attachment(const TextureDelegate& settings);

		VkClearValue getClearValue() const;
		VkFormat getFormat() const;

		void allocate(LogicalDevice& device, int width, int height, Allocator& allocator);

};