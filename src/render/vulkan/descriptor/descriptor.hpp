#pragma once

#include "external.hpp"

class DescriptorSetLayout;
class Buffer;
class Texture;
class AccelStruct;
class ImageView;

class DescriptorSet {

	private:

		VkDevice vk_device;
		VkDescriptorSet vk_set;
		const DescriptorSetLayout* layout;

	public:

		DescriptorSet() {}
		DescriptorSet(VkDevice device, VkDescriptorSet set, const DescriptorSetLayout* layout);

		/**
		 * Update one descriptor binding to point to the given buffer or buffer section,
		 * for the given descriptor binding site to be compatible the layout from which
		 * this set was created must configure the binding to match this resource type.
		 *
		 * @param binding descriptor set binding index to update
		 * @param sampler buffer to update it to
		 * @param length  length of the buffer (or part of it) in bytes
		 * @param offset  offset in the buffer where the section starts (in bytes)
		 */
		void buffer(int binding, const Buffer& buffer, size_t length, int offset = 0);

		/**
		 * Update one descriptor binding to point to the given sampler,
		 * for the given descriptor binding site to be compatible the layout
		 * from which this set was created must configure the binding to match this resource type.
		 *
		 * @param binding descriptor set binding index to update
		 * @param texture texture to update it to
		 * @param layout  layout the image will have when the set will be bound
		 */
		void sampler(int binding, const Texture& texture, VkImageLayout layout);

		/**
		 * Update one descriptor binding to point to the given sampler array,
		 * for the given descriptor binding site to be compatible the layout
		 * from which this set was created must configure the binding to match this resource type.
		 *
		 * @param binding  descriptor set binding index to update
		 * @param textures array of textures to update it to
		 * @param layout   layout the image will have when the set will be bound
		 */
		void samplerArray(int binding, const std::vector<Texture>& textures, VkImageLayout layout);

		/**
		 * Update one descriptor binding to point to the given image view,
		 * for the given descriptor binding site to be compatible the layout
		 * from which this set was created must configure the binding to match this resource type.
		 *
		 * @param binding descriptor set binding index to update
		 * @param view    image view of a texture to update it to
		 * @param layout  layout the image will have when the set will be bound
		 */
		void view(int binding, const ImageView& view, VkImageLayout layout);

		/**
		 * Update one descriptor binding to point to the given Top
		 * Level Acceleration Structure, for the given descriptor binding site to
		 * be compatible the layout from which this set was created must configure
		 * the binding to match this resource type.
		 *
		 * @param binding   descriptor set binding index to update
		 * @param structure texture to update it to
		 */
		void structure(int binding, const AccelStruct& structure);

		/// Get the handle to the underlying vulkan object
		VkDescriptorSet getHandle() const;

};