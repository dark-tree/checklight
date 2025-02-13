
#include "shader.hpp"
#include "render/vulkan/setup/device.hpp"
#include "render/vulkan/setup/debug.hpp"

Shader::Shader(LogicalDevice& device, const uint32_t* data, uint32_t size, VkShaderStageFlagBits stage, const std::string& unit)
:  vk_device(device.getHandle()), vk_stage(stage) {

	VkShaderModuleCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = size;
	create_info.pCode = data;

	if (vkCreateShaderModule(vk_device, &create_info, nullptr, &vk_module) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create shader module!"};
	}

	VulkanDebug::setDebugName(device.getHandle(), VK_OBJECT_TYPE_SHADER_MODULE, vk_module, unit.c_str());

}

void Shader::close() {
	vkDestroyShaderModule(vk_device, vk_module, nullptr);
}

VkPipelineShaderStageCreateInfo Shader::getConfig() const {
	VkPipelineShaderStageCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	create_info.stage = vk_stage;
	create_info.module = getHandle();
	create_info.pName = "main";

	return create_info;
}

VkShaderModule Shader::getHandle() const {
	return vk_module;
}