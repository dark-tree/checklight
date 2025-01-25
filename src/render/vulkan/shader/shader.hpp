#pragma once

#include "external.hpp"

class LogicalDevice;

class Shader {

	private:

		VkDevice vk_device;
		VkShaderModule vk_module;
		VkShaderStageFlagBits vk_stage;

	public:

		Shader() = default;
		Shader(LogicalDevice& device, const uint32_t* data, uint32_t size, VkShaderStageFlagBits stage);

		void close();

		/**
		 * @brief Get pipeline shader config
		 * Returns the config needed to use shader in a pipeline
		 */
		VkPipelineShaderStageCreateInfo getConfig() const;

};
