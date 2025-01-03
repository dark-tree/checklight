#pragma once

#include "external.hpp"
#include "texture.hpp"

class Attachment {

	private:

		Texture texture;
		VkClearValue clear;

	public:

		Attachment(const Texture& texture, VkClearValue clear);

		VkClearValue getClearValue() const;

};
