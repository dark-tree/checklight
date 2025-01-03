
#include "attachment.hpp"

Attachment::Attachment(const Texture& texture, VkClearValue clear)
: texture(texture), clear(clear) {}

VkClearValue Attachment::getClearValue() const {
	return clear;
}