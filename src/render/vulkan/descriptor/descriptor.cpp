
#include "descriptor.hpp"
#include "layout.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/vulkan/buffer/texture.hpp"
#include "render/vulkan/raytrace/struct.hpp"

/*
 * DescriptorSet
 */

DescriptorSet::DescriptorSet(VkDevice device, VkDescriptorSet set, const DescriptorSetLayout* layout)
: vk_device(device), vk_set(set), layout(layout) {}

void DescriptorSet::buffer(int binding, const Buffer& buffer, size_t length, int offset) {

	const VkDescriptorType type = layout->getType(binding);

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

void DescriptorSet::sampler(int binding, const Texture& texture) {

	const VkDescriptorType type = layout->getType(binding);

	VkDescriptorImageInfo info {};
	info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	info.imageView = texture.getView();
	info.sampler = texture.getSampler();

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
	VkAccelerationStructureKHR tlas = structure.getHandle();

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