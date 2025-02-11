
#include "struct.hpp"

/*
 * TraceStruct
 */

TraceStruct::TraceStruct(const Buffer& buffer, VkAccelerationStructureKHR vk_structure)
: buffer(buffer), vk_structure(vk_structure) {}

VkAccelerationStructureKHR TraceStruct::getHandle() const {
	return vk_structure;
}