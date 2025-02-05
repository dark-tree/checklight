
#include "struct.hpp"

AccelerationStructure::AccelerationStructure(const Buffer& buffer, VkAccelerationStructureKHR vk_structure)
: buffer(buffer), vk_structure(vk_structure) {}