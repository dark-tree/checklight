
#include "pool.hpp"
#include "external.hpp"
#include "descriptor.hpp"
#include "layout.hpp"
#include "render/vulkan/device.hpp"

/*
 * DescriptorPool
 */

DescriptorPool::DescriptorPool(VkDescriptorPool vk_pool, VkDevice vk_device)
: vk_pool(vk_pool), vk_device(vk_device) {}

void DescriptorPool::close() {
	vkDestroyDescriptorPool(vk_device, vk_pool, nullptr);
}

void DescriptorPool::reset() {
	vkResetDescriptorPool(vk_device, vk_pool, 0);
}

DescriptorSet DescriptorPool::allocate(const DescriptorSetLayout& layout) const {
	VkDescriptorSet set;

	VkDescriptorSetLayout vk_layout = layout.getHandle();

	VkDescriptorSetAllocateInfo alloc_info {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.pSetLayouts = &vk_layout;
	alloc_info.descriptorSetCount = 1;
	alloc_info.descriptorPool = vk_pool;

	if (vkAllocateDescriptorSets(vk_device, &alloc_info, &set) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to allocate descriptor set!"};
	}

	return {vk_device, set, &layout};
}

/*
 * DescriptorPoolBuilder
 */

DescriptorPoolBuilder& DescriptorPoolBuilder::addDynamic(const DescriptorSetLayout& layout, int count) {
	dynamic_sets += count;
	while (count --> 0) layout.appendUsedTypes(dynamics);
	return *this;
}

DescriptorPoolBuilder& DescriptorPoolBuilder::addStatic(const DescriptorSetLayout& layout, int count) {
	static_sets += count;
	while (count --> 0) layout.appendUsedTypes(statics);
	return *this;
}

DescriptorPool DescriptorPoolBuilder::done(LogicalDevice device, uint32_t sets) const {

	std::unordered_map<VkDescriptorType, VkDescriptorPoolSize> types;

	for (VkDescriptorType type : statics) {
		VkDescriptorPoolSize& pool = types[type];

		pool.type = type;
		pool.descriptorCount += 1;
	}

	for (VkDescriptorType type : dynamics) {
		VkDescriptorPoolSize& pool = types[type];

		pool.type = type;
		pool.descriptorCount += sets;
	}

	std::vector<VkDescriptorPoolSize> pools;
	pools.reserve(types.size());

	for (std::pair<VkDescriptorType, VkDescriptorPoolSize> pair : types) {
		pools.push_back(pair.second);
	}

	VkDescriptorPoolCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = pools.size();
	create_info.pPoolSizes = pools.data();
	create_info.maxSets = sets * dynamic_sets + static_sets;

	VkDescriptorPool pool;

	if (vkCreateDescriptorPool(device.getHandle(), &create_info, nullptr, &pool) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create descriptor pool!"};
	}

	return {pool, device.getHandle()};
}

DescriptorPoolBuilder DescriptorPoolBuilder::begin() {
	return {};
}