
#include "raytrace.hpp"
#include "render/vulkan/setup/proxy.hpp"

/*
 * RaytracePipelineBuilder
 */

RaytracePipelineBuilder::RaytracePipelineBuilder(const LogicalDevice& device)
: AbstractPipelineBuilder<RaytracePipelineBuilder>(device) {
	this->limit = device.getPhysical()->getMaxRaytraceRecursionDepth();
}

RaytracePipelineBuilder RaytracePipelineBuilder::of(const LogicalDevice& device) {
	return {device};
}

RaytracePipelineBuilder& RaytracePipelineBuilder::withRecursionDepth(int depth) {
	if (depth > limit) {
		throw std::runtime_error {"Requested recursion depth of " + std::to_string(depth) + " is bigger that the supported value of " + std::to_string(limit)};
	}

	this->depth = depth;
	return *this;
}

RaytracePipelineBuilder& RaytracePipelineBuilder::withShaderLayout(const ShaderTableLayout& layout) {
	this->layout = layout;
	return *this;
}

GraphicsPipeline RaytracePipelineBuilder::build() {

	AbstractPipelineBuilder::finalize();
	VkPipelineLayout pipeline_layout = createPipelineLayout();

	VkRayTracingPipelineCreateInfoKHR create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
	create_info.pNext = nullptr;
	create_info.layout = pipeline_layout;

	// shaders
	create_info.pStages = layout.stages.data();
	create_info.stageCount = layout.stages.size();
	create_info.pGroups = layout.groups.data();
	create_info.groupCount = layout.groups.size();

	create_info.pDynamicState = &dynamic;
	create_info.maxPipelineRayRecursionDepth = depth;

	VkPipeline pipeline;

	if (Proxy::vkCreateRayTracingPipelinesKHR(vk_device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create raytrace pipeline!"};
	}

	return {PipelineType::RAYTRACE, pipeline, pipeline_layout, vk_device};

}
