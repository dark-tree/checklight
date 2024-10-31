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
		VkPipelineShaderStageCreateInfo getConfig() const;

};
