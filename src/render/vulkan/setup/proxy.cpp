
#include "proxy.hpp"
#include "instance.hpp"

/*
 * Proxy
 */

#define LOAD_FUNCTION(source, name) Proxy::name = source.getFunction<PFN_##name>(#name)
#define IMPL_FUNCTION(name) PFN_##name Proxy::name = nullptr

IMPL_FUNCTION(vkGetPhysicalDeviceFeatures2);
IMPL_FUNCTION(vkGetPhysicalDeviceProperties2);
IMPL_FUNCTION(vkCreateDebugUtilsMessengerEXT);
IMPL_FUNCTION(vkDestroyDebugUtilsMessengerEXT);
IMPL_FUNCTION(vkSetDebugUtilsObjectNameEXT);
IMPL_FUNCTION(vkCmdBeginDebugUtilsLabelEXT);
IMPL_FUNCTION(vkCmdEndDebugUtilsLabelEXT);
IMPL_FUNCTION(vkCmdInsertDebugUtilsLabelEXT);
IMPL_FUNCTION(vkCmdTraceRaysKHR);
IMPL_FUNCTION(vkCreateRayTracingPipelinesKHR);
IMPL_FUNCTION(vkGetRayTracingShaderGroupHandlesKHR);
IMPL_FUNCTION(vkCreateAccelerationStructureKHR);
IMPL_FUNCTION(vkGetAccelerationStructureBuildSizesKHR);
IMPL_FUNCTION(vkCmdBuildAccelerationStructuresKHR);
IMPL_FUNCTION(vkCmdCopyAccelerationStructureKHR);
IMPL_FUNCTION(vkDestroyAccelerationStructureKHR);
IMPL_FUNCTION(vkGetAccelerationStructureDeviceAddressKHR);
IMPL_FUNCTION(vkGetBufferDeviceAddress);
IMPL_FUNCTION(vkCmdWriteAccelerationStructuresPropertiesKHR);

void Proxy::loadMessengerFunctions(Instance& instance) {
	LOAD_FUNCTION(instance, vkCreateDebugUtilsMessengerEXT);
	LOAD_FUNCTION(instance, vkDestroyDebugUtilsMessengerEXT);
}

void Proxy::loadDebugDeviceFunctions(LogicalDevice& device) {
	LOAD_FUNCTION(device, vkSetDebugUtilsObjectNameEXT);
	LOAD_FUNCTION(device, vkCmdBeginDebugUtilsLabelEXT);
	LOAD_FUNCTION(device, vkCmdEndDebugUtilsLabelEXT);
	LOAD_FUNCTION(device, vkCmdInsertDebugUtilsLabelEXT);
}

void Proxy::loadInstanceFunctions(Instance& instance) {
	LOAD_FUNCTION(instance, vkGetPhysicalDeviceFeatures2);
	LOAD_FUNCTION(instance, vkGetPhysicalDeviceProperties2);
}

void Proxy::loadDeviceFunctions(LogicalDevice& device) {
	LOAD_FUNCTION(device, vkCmdTraceRaysKHR);
	LOAD_FUNCTION(device, vkCreateRayTracingPipelinesKHR);
	LOAD_FUNCTION(device, vkCreateAccelerationStructureKHR);
	LOAD_FUNCTION(device, vkGetBufferDeviceAddress);
	LOAD_FUNCTION(device, vkGetAccelerationStructureBuildSizesKHR);
	LOAD_FUNCTION(device, vkCmdBuildAccelerationStructuresKHR);
	LOAD_FUNCTION(device, vkCmdCopyAccelerationStructureKHR);
	LOAD_FUNCTION(device, vkDestroyAccelerationStructureKHR);
	LOAD_FUNCTION(device, vkGetAccelerationStructureDeviceAddressKHR);
	LOAD_FUNCTION(device, vkCmdWriteAccelerationStructuresPropertiesKHR);
	LOAD_FUNCTION(device, vkGetRayTracingShaderGroupHandlesKHR);
}