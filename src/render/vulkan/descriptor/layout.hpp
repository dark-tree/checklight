#pragma once

#include "external.hpp"

class LogicalDevice;

struct DescriptorType {
	VkDescriptorType vk_type;
	int count;
	bool enabled;

	inline DescriptorType() noexcept
	: enabled(false) {}
};

class DescriptorSetLayout {

	private:

		VkDevice vk_device;
		VkDescriptorSetLayout vk_layout;
		std::vector<DescriptorType> types;

	public:

		DescriptorSetLayout() = default;
		DescriptorSetLayout(VkDevice device, VkDescriptorSetLayout layout, const std::vector<DescriptorType>& types);

		/// Free the underlying vulkan object
		void close();

		/// Returns the VkDescriptorType of the specified binding index as defined in DescriptorSetLayoutBuilder
		VkDescriptorType getType(uint32_t index) const;

		/// Append all descriptor types used in this layout to the given list
		void appendUsedTypes(std::vector<VkDescriptorType>& vector) const;

		/// Get the vulkan object handle
		VkDescriptorSetLayout getHandle() const;
};

class DescriptorSetLayoutBuilder {

	private:

		VkDescriptorSetLayoutCreateFlags vk_flags;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		std::vector<DescriptorType> types;
		std::set<uint32_t> indices;

		void addBindingTypeMapping(uint32_t index, VkDescriptorType type, int count);

	public:

		DescriptorSetLayoutBuilder() = default;
		DescriptorSetLayoutBuilder(VkDescriptorSetLayoutCreateFlags flags = 0);

		/**
		 * Adds a new descriptor binding
		 *
		 * @param index  the binding index, must match the one specified in the shader and the one in the allocated descriptor set
		 * @param type   the type of the data that will be bound to this binding
		 * @param shader the shader stages where the binding will be made available
		 * @param count  the number of descriptors in the binding
		 */
		DescriptorSetLayoutBuilder& descriptor(uint32_t index, VkDescriptorType type, VkShaderStageFlags shader, uint32_t count = 1);

		/**
		 * Finish and build the layout object
		 */
		DescriptorSetLayout done(LogicalDevice device) const;

		/**
		 * Get a new DescriptorSetLayoutBuilder
		 */
		static DescriptorSetLayoutBuilder begin(VkDescriptorSetLayoutCreateFlags flags = 0);

};