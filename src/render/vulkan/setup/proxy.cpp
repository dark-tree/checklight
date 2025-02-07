
#include "proxy.hpp"
#include "instance.hpp"

/*
 * Proxy
 */

#define LOAD_FUNCTION(source, name) Proxy::name = source.getFunction<PFN_##name>(#name)

PFN_vkGetPhysicalDeviceFeatures2KHR Proxy::vkGetPhysicalDeviceFeatures2KHR = nullptr;
PFN_vkCreateDebugUtilsMessengerEXT Proxy::vkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT Proxy::vkDestroyDebugUtilsMessengerEXT = nullptr;
PFN_vkSetDebugUtilsObjectNameEXT Proxy::vkSetDebugUtilsObjectNameEXT = nullptr;
PFN_vkCmdBeginDebugUtilsLabelEXT Proxy::vkCmdBeginDebugUtilsLabelEXT = nullptr;
PFN_vkCmdEndDebugUtilsLabelEXT Proxy::vkCmdEndDebugUtilsLabelEXT = nullptr;
PFN_vkCmdInsertDebugUtilsLabelEXT Proxy::vkCmdInsertDebugUtilsLabelEXT = nullptr;
PFN_vkCmdTraceRaysKHR Proxy::vkCmdTraceRaysKHR = nullptr;
PFN_vkCreateRayTracingPipelinesKHR Proxy::vkCreateRayTracingPipelinesKHR = nullptr;
PFN_vkCreateAccelerationStructureKHR Proxy::vkCreateAccelerationStructureKHR = nullptr;
PFN_vkGetBufferDeviceAddressKHR Proxy::vkGetBufferDeviceAddressKHR = nullptr;

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
	LOAD_FUNCTION(instance, vkGetPhysicalDeviceFeatures2KHR);
}

void Proxy::loadDeviceFunctions(LogicalDevice& device) {

}

void Proxy::loadRaytraceFunctions(LogicalDevice& device) {
	LOAD_FUNCTION(device, vkCmdTraceRaysKHR);
	LOAD_FUNCTION(device, vkCreateRayTracingPipelinesKHR);
	LOAD_FUNCTION(device, vkCreateAccelerationStructureKHR);
	LOAD_FUNCTION(device, vkGetBufferDeviceAddressKHR);
}