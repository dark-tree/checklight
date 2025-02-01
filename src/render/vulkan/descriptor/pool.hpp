
#pragma once

#include "external.hpp"

class DescriptorSetLayout;
class DescriptorSet;
class LogicalDevice;

class DescriptorPool {

	private:

		VkDescriptorPool vk_pool;
		VkDevice vk_device;

	public:

		DescriptorPool() = default;
		DescriptorPool(VkDescriptorPool vk_pool, VkDevice vk_device);

		/**
		 * Free the underlying vulkan object
		 */
		void close();

		/**
		 * Free all the layouts allocated from this pool
		 */
		void reset();

		/**
		 * Allocate a new layout from the pool
		 */
		DescriptorSet allocate(const DescriptorSetLayout& layout) const;
};

class DescriptorPoolBuilder {

	private:

		int dynamic_sets = 0;
		int static_sets = 0;

		std::vector<VkDescriptorType> dynamics;
		std::vector<VkDescriptorType> statics;

	public:

		/**
		 * Add a layout that will be allocated for each concurrent frame
		 */
		DescriptorPoolBuilder& addDynamic(const DescriptorSetLayout& layout, int count = 1);

		/**
		 * Add a layout that will be allocated only once per application
		 */
		DescriptorPoolBuilder& addStatic(const DescriptorSetLayout& layout, int count = 1);

		/**
		 * Finish pool creation for the given number of concurrent frames
		 */
		DescriptorPool done(LogicalDevice device, uint32_t sets) const;

		/**
		 * Get a new DescriptorSetLayoutBuilder
		 */
		static DescriptorPoolBuilder begin();

};