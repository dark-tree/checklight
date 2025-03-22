
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
		FAULT("Failed to create shader module!");
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

Shader Shader::loadFromFile(LogicalDevice& device, const std::string& path, VkShaderStageFlagBits stage) {

	std::string blob = "assets/shader/" + path + ".spv";
	std::ifstream file(blob, std::ios::binary | std::ios::ate);

	if (!file) {
		FAULT("Failed to open compiled shader: '", blob, "'");
	}

	uint32_t size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> buffer(size);
	if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
		FAULT("Failed to read compiled shader: '", blob, "'");
	}

	return {device, reinterpret_cast<const uint32_t*>(buffer.data()), size, stage, path};
}