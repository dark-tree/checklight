
#include "struct.hpp"
#include "render/vulkan/setup/proxy.hpp"

/*
 * AccelStruct
 */

AccelStruct::AccelStruct(const Buffer& buffer, VkAccelerationStructureKHR vk_structure)
: buffer(buffer), vk_structure(vk_structure) {}

void AccelStruct::close(const LogicalDevice& device) {
	if (buffer.allocated()) {
		Proxy::vkDestroyAccelerationStructureKHR(device.getHandle(), vk_structure, nullptr);
		buffer.close();
	}
}

VkAccelerationStructureKHR AccelStruct::getHandle() const {
	return vk_structure;
}