
#include "vertex.hpp"

/*
 * BindingLayout
 */

BindingLayout::BindingLayout(uint32_t stride, std::vector<VkVertexInputAttributeDescription> attributes, VkVertexInputRate rate)
: stride(stride), attributes(attributes), vk_rate(rate) {}

std::vector<VkVertexInputAttributeDescription> BindingLayout::getAttributes() const {
	return attributes;
}

uint32_t BindingLayout::getStride() const {
	return stride;
}

VkVertexInputRate BindingLayout::getRate() const {
	return vk_rate;
}

/*
 * BindingLayoutBuilder
 */

BindingLayoutBuilder::BindingLayoutBuilder(VkVertexInputRate rate)
: offset(0), attributes(), vk_rate(rate) {}

BindingLayoutBuilder BindingLayoutBuilder::begin(VkVertexInputRate rate) {
	return {rate};
}

BindingLayoutBuilder& BindingLayoutBuilder::attribute(uint32_t location, VkFormat format) {

	VkVertexInputAttributeDescription description {};
	description.location = location;
	description.format = format;
	description.offset = offset;

	offset += getFormatInfo(format).size;
	attributes.push_back(description);
	return *this;
}

BindingLayout BindingLayoutBuilder::done() {
	return {offset, attributes, vk_rate};
}