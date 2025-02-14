
#include "pipeline.hpp"
#include "render.hpp"
#include "render/vulkan/shader/shader.hpp"
#include "render/vulkan/descriptor/vertex.hpp"
#include "render/vulkan/descriptor/layout.hpp"
#include "render/vulkan/setup/device.hpp"

/*
 * GraphicsPipeline
 */

GraphicsPipeline::GraphicsPipeline(PipelineType type, VkPipeline vk_pipeline, VkPipelineLayout vk_layout, VkDevice vk_device)
: type(type), vk_pipeline(vk_pipeline), vk_layout(vk_layout), vk_device(vk_device) {}

void GraphicsPipeline::close() {
	vkDestroyPipeline(vk_device, vk_pipeline, nullptr);
	vkDestroyPipelineLayout(vk_device, vk_layout, nullptr);
}

VkPipeline GraphicsPipeline::getHandle() const {
	return vk_pipeline;
}

VkPipelineLayout GraphicsPipeline::getLayout() const {
	return vk_layout;
}

PipelineType GraphicsPipeline::getType() const {
	return type;
}

VkPipelineBindPoint GraphicsPipeline::getBindPoint() {
	return (type == PipelineType::RAYTRACE) ? VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR : VK_PIPELINE_BIND_POINT_GRAPHICS;
}
