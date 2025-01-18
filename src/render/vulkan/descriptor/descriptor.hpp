#pragma once

#include "external.hpp"

class DescriptorSetLayout;
class Buffer;
class Texture;

class DescriptorSet {

	private:

		VkDevice vk_device;
		VkDescriptorSet vk_set;
		const DescriptorSetLayout* layout;

	public:

		DescriptorSet() {}
		DescriptorSet(VkDevice device, VkDescriptorSet set, const DescriptorSetLayout* layout);

		/**
		 * Update one descriptor binding to point to the given buffer or buffer section
		 *
		 * @param binding the descriptor set binding index to update
		 * @param sampler the buffer to update it to
		 * @param length the length of the buffer (or part of it) in bytes
		 * @param offset the offset in the buffer where the section starts (in bytes)
		 */
		void buffer(int binding, const Buffer& buffer, size_t length, int offset = 0);

		/**
		 * Update one descriptor binding to point to the given sampler
		 *
		 * @param binding the descriptor set binding index to update
		 * @param texture the texture to update it to
		 */
		void sampler(int binding, const Texture& texture);

};