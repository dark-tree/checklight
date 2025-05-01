
#include "layout.hpp"
#include "render/vulkan/setup/device.hpp"

/*
 * DescriptorSetLayout
 */

DescriptorSetLayout::DescriptorSetLayout(VkDevice device, VkDescriptorSetLayout layout, const std::vector<DescriptorType>& types)
: vk_device(device), vk_layout(layout), types(types) {}

void DescriptorSetLayout::close() {
	vkDestroyDescriptorSetLayout(vk_device, vk_layout, nullptr);
}

VkDescriptorType DescriptorSetLayout::getType(uint32_t index) const {
	if (index >= types.size()) {
		FAULT("Descriptor index out of defined range!");
	}

	return types.at(index).vk_type;
}

void DescriptorSetLayout::appendUsedTypes(std::vector<VkDescriptorType>& vector) const {
	vector.reserve(types.size());

	for (DescriptorType type : types) {
		if (type.enabled) {
			vector.reserve(vector.size() + type.count);

			for (int i = 0; i < type.count; i++) {
				vector.push_back(type.vk_type);
			}
		}
	}
}

VkDescriptorSetLayout DescriptorSetLayout::getHandle() const {
	return vk_layout;
}

/*
 * DescriptorSetLayoutBuilder
 */

void DescriptorSetLayoutBuilder::addBindingTypeMapping(uint32_t index, VkDescriptorType type, int count) {
	types.resize(index + 1);
	DescriptorType& descriptor = types[index];

	descriptor.count = count;
	descriptor.vk_type = type;
	descriptor.enabled = true;
}

DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder(VkDescriptorSetLayoutCreateFlags flags)
: vk_flags(flags) {}

DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::descriptor(uint32_t index, VkDescriptorType type, VkShaderStageFlags shader, uint32_t count) {

	if (indices.contains(index)) {
		FAULT("Attempted to redefine descriptor set binding index!");
	}

	VkDescriptorSetLayoutBinding binding {};
	binding.binding = index;
	binding.descriptorType = type;
	binding.stageFlags = shader;
	binding.descriptorCount = count;

	bindings.push_back(binding);
	addBindingTypeMapping(index, type, count);
	indices.insert(index);

	return *this;
}

DescriptorSetLayout DescriptorSetLayoutBuilder::done(LogicalDevice device) const {
	VkDescriptorSetLayout layout;

	VkDescriptorSetLayoutCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	create_info.flags = vk_flags;
	create_info.bindingCount = bindings.size();
	create_info.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device.getHandle(), &create_info, nullptr, &layout) != VK_SUCCESS) {
		FAULT("Failed to create descriptor set!");
	}

	return {device.getHandle(), layout, types};
}

DescriptorSetLayoutBuilder DescriptorSetLayoutBuilder::begin(VkDescriptorSetLayoutCreateFlags flags) {
	return {flags};
}