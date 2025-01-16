#pragma once

#include "external.hpp"

/**
 * Wrapper around Vulkan vertex attribute array
 */
class BindingLayout {

	private:

		uint32_t stride;
		std::vector<VkVertexInputAttributeDescription> attributes;
		VkVertexInputRate vk_rate;

	public:

		BindingLayout() = default;
		BindingLayout(uint32_t stride, std::vector<VkVertexInputAttributeDescription> attributes, VkVertexInputRate rate);

		/**
		 * Returns a copy of the attribute vector
		 */
		std::vector<VkVertexInputAttributeDescription> getAttributes() const;

		/**
		 * Returns the stride of the binding
		 */
		uint32_t getStride() const;

		/**
		 * Returns the input interval
		 */
		VkVertexInputRate getRate() const;

};

class BindingLayoutBuilder {

	private:

		uint32_t offset;
		std::vector<VkVertexInputAttributeDescription> attributes;
		VkVertexInputRate vk_rate;

	public:

		BindingLayoutBuilder(VkVertexInputRate rate);

		static BindingLayoutBuilder begin(VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX);

	public:

		/**
		 * Add binding attribute
		 */
		BindingLayoutBuilder& attribute(uint32_t location, VkFormat format);

		/**
		 * Finish and create the binding object
		 */
		BindingLayout done();

};
