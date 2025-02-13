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
		Shader(LogicalDevice& device, const uint32_t* data, uint32_t size, VkShaderStageFlagBits stage, const std::string& unit);

		/// Shader live time is managed by the compiler, don't free them yourself!
		void close();

		/// Get the pipeline config of this shader module
		VkPipelineShaderStageCreateInfo getConfig() const;

		/// Get the underlying vulkan shader module handle
		VkShaderModule getHandle() const;

};
