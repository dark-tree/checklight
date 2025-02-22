
#include "descriptor.hpp"
#include "layout.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/vulkan/buffer/texture.hpp"
#include "render/vulkan/raytrace/struct.hpp"
#include "render/vulkan/setup/debug.hpp"

/*
 * DescriptorSet
 */

DescriptorSet::DescriptorSet(VkDevice device, VkDescriptorSet set, const DescriptorSetLayout* layout)
: vk_device(device), vk_set(set), layout(layout) {}

void DescriptorSet::buffer(int binding, const Buffer& buffer, size_t length, int offset) {

	const VkDescriptorType type = layout->getType(binding);
	const VkBuffer vk_buffer = buffer.getHandle();
	VulkanDebug::assertAlive(vk_buffer);

	VkDescriptorBufferInfo info {};
	info.buffer = buffer.getHandle();
	info.offset = offset;
	info.range = length;

	VkWriteDescriptorSet write {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = vk_set;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = type;
	write.descriptorCount = 1;
	write.pBufferInfo = &info;

	vkUpdateDescriptorSets(vk_device, 1, &write, 0, nullptr);

}

void DescriptorSet::sampler(int binding, const Texture& texture, VkImageLayout image_layout) {

	const VkDescriptorType type = layout->getType(binding);
	const VkSampler vk_sampler = texture.getSampler();
	VulkanDebug::assertAlive(vk_sampler);

	VkDescriptorImageInfo info {};
	info.imageLayout = image_layout;
	info.imageView = texture.getView();
	info.sampler = vk_sampler;

	VkWriteDescriptorSet write {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = vk_set;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = type;
	write.descriptorCount = 1;
	write.pImageInfo = &info;

	vkUpdateDescriptorSets(vk_device, 1, &write, 0, nullptr);

}

void DescriptorSet::samplerArray(int binding, const std::vector<Texture>& textures, VkImageLayout image_layout) {

	const VkDescriptorType type = layout->getType(binding);
	std::vector<VkDescriptorImageInfo> infos(textures.size());

	for (size_t i = 0; i < textures.size(); i++) {
		const Texture& texture = textures[i];
		const VkSampler vk_sampler = texture.getSampler();
		VulkanDebug::assertAlive(vk_sampler);

		VkDescriptorImageInfo& info = infos[i];
		info.imageLayout = image_layout;
		info.imageView = texture.getView();
		info.sampler = vk_sampler;
	}

	VkWriteDescriptorSet write {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = vk_set;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = type;
	write.descriptorCount = static_cast<uint32_t>(textures.size());
	write.pImageInfo = infos.data();

	vkUpdateDescriptorSets(vk_device, 1, &write, 0, nullptr);

}

void DescriptorSet::view(int binding, const ImageView& view, VkImageLayout image_layout) {

	const VkDescriptorType type = layout->getType(binding);
	const VkImageView vk_view = view.getHandle();
	VulkanDebug::assertAlive(vk_view);

	VkDescriptorImageInfo info {};
	info.imageLayout = image_layout;
	info.imageView = vk_view;

	VkWriteDescriptorSet write {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = vk_set;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = type;
	write.descriptorCount = 1;
	write.pImageInfo = &info;

	vkUpdateDescriptorSets(vk_device, 1, &write, 0, nullptr);

}

void DescriptorSet::structure(int binding, const AccelStruct& structure) {

	const VkDescriptorType type = layout->getType(binding);
	const VkAccelerationStructureKHR tlas = structure.getHandle();
	VulkanDebug::assertAlive(tlas);

	VkWriteDescriptorSetAccelerationStructureKHR info {};
	info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
	info.accelerationStructureCount = 1;
	info.pAccelerationStructures = &tlas;

	VkWriteDescriptorSet write {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = &info;
	write.dstSet = vk_set;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = type;
	write.descriptorCount = 1;

	vkUpdateDescriptorSets(vk_device, 1, &write, 0, nullptr);

}

VkDescriptorSet DescriptorSet::getHandle() const {
	return vk_set;
}