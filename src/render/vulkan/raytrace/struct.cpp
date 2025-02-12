
#include "struct.hpp"

/*
 * AccelStruct
 */

AccelStruct::AccelStruct(const Buffer& buffer, VkAccelerationStructureKHR vk_structure)
: buffer(buffer), vk_structure(vk_structure) {}

VkAccelerationStructureKHR AccelStruct::getHandle() const {
	return vk_structure;
}