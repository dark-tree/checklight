#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/buffer.hpp"

class Allocator;
class LogicalDevice;
class GraphicsPipeline;

class ShaderTable;

/**
 * A configured set of shaders, to be used it needs to be allocated
 * first, creating a ShaderTable object
 */
class ShaderTableLayout {

	private:

		friend class ShaderTableBuilder;
		friend class RaytracePipelineBuilder;

		int generate, miss, hit;

		std::vector<VkPipelineShaderStageCreateInfo> stages;
		std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups;

	public:

		/// Create a ShaderTable object needed for raytracing
		ShaderTable allocate(const LogicalDevice& device, Allocator& allocator, GraphicsPipeline& pipeline);

};

/**
 * A buffer holding shader handles to be used during
 * raytracing process
 */
class ShaderTable {

	private:

		friend class CommandRecorder;

		Buffer buffer;

		VkStridedDeviceAddressRegionKHR vk_region_generate {};
		VkStridedDeviceAddressRegionKHR vk_region_miss {};
		VkStridedDeviceAddressRegionKHR vk_region_hit {};
		VkStridedDeviceAddressRegionKHR vk_region_call {};

	public:

		ShaderTable() = default;
		ShaderTable(const LogicalDevice& device, Allocator& allocator, GraphicsPipeline& pipeline, int generate, int miss, int hit, int call);

		/// Free the buffer backing the shader table
		void close();

};
