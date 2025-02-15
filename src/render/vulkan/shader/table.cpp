
#include "table.hpp"
#include "render/vulkan/setup/device.hpp"
#include "render/vulkan/setup/allocator.hpp"
#include "render/vulkan/setup/proxy.hpp"
#include "render/vulkan/pass/pipeline.hpp"
#include "shared/math.hpp"

/*
 * ShaderTableLayout
 */

ShaderTable ShaderTableLayout::allocate(const LogicalDevice& device, Allocator& allocator, GraphicsPipeline& pipeline) {
	return {device, allocator, pipeline, generate, miss, hit, 0};
}

/*
 * ShaderTable
 */

ShaderTable::ShaderTable(const LogicalDevice& device, Allocator& allocator, GraphicsPipeline& pipeline, int generate, int miss, int hit, int call) {

	if (pipeline.getType() != PipelineType::RAYTRACE) {
		throw std::runtime_error {"Expected a raytrace pipeline!"};
	}

	const VkStructureType type = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
	auto properties = (VkPhysicalDeviceRayTracingPipelinePropertiesKHR*) device.getPhysical()->getProperties(type);

	int total_handles = generate + miss + hit + call;
	int handle_size = math::alignUp(properties->shaderGroupHandleSize, properties->shaderGroupHandleAlignment);
	int group_align = properties->shaderGroupBaseAlignment;

	vk_region_generate.size = math::alignUp(generate * handle_size, group_align);
	vk_region_generate.stride = vk_region_generate.size; // The stride of vk_region_generate must be equal to its size member

	vk_region_miss.size = math::alignUp(miss * handle_size, group_align);
	vk_region_miss.stride = handle_size;

	vk_region_hit.size = math::alignUp(hit * handle_size, group_align);
	vk_region_hit.stride = handle_size;

	vk_region_call.size = math::alignUp(call * handle_size, group_align);
	vk_region_call.stride = handle_size;

	// load group handles into a opaque byte buffer
	std::vector<uint8_t> handles;
	handles.resize(properties->shaderGroupHandleSize * total_handles);
	if (Proxy::vkGetRayTracingShaderGroupHandlesKHR(device.getHandle(), pipeline.getHandle(), 0, total_handles, handles.size(), handles.data()) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to load shader group handles!"};
	}

	VkDeviceSize buffer_size = vk_region_generate.size + vk_region_miss.size + vk_region_hit.size + vk_region_call.size;
	VkBufferUsageFlags flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;
	buffer = allocator.allocateBuffer(Memory::STAGED, buffer_size, flags, "Shader Table");

	auto* data = static_cast<uint8_t*>(buffer.getAllocation().map());
	size_t handle = 0;
	size_t offset = 0;

	auto copyRegion = [&] (int count, VkStridedDeviceAddressRegionKHR& region, VkDeviceAddress address) {
		for (int i = 0; i < count; i ++) {
			memcpy(data + offset, handles.data() + handle * handle_size, handle_size);
			offset += region.stride;
			handle ++;
		}

		offset = 0;
		data += region.size;
		region.deviceAddress = address;
	};

	VkDeviceAddress address = device.getAddress(buffer);

	copyRegion(generate, vk_region_generate, address);
	copyRegion(miss, vk_region_miss, address);
	copyRegion(hit, vk_region_hit, address);
	copyRegion(call, vk_region_call, address);

	buffer.getAllocation().unmap();

}

void ShaderTable::close() {
	buffer.close();

	vk_region_generate.deviceAddress = 0;
	vk_region_miss.deviceAddress = 0;
	vk_region_hit.deviceAddress = 0;
	vk_region_call.deviceAddress = 0;
}