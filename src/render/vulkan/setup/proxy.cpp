
#include "proxy.hpp"
#include "instance.hpp"

// VK_KHR_get_physical_device_properties2
PFN_vkGetPhysicalDeviceFeatures2KHR Proxy::vkGetPhysicalDeviceFeatures2KHR = nullptr;

// VK_EXT_debug_utils
PFN_vkCreateDebugUtilsMessengerEXT Proxy::vkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT Proxy::vkDestroyDebugUtilsMessengerEXT = nullptr;
PFN_vkSetDebugUtilsObjectNameEXT Proxy::vkSetDebugUtilsObjectNameEXT = nullptr;
PFN_vkCmdBeginDebugUtilsLabelEXT Proxy::vkCmdBeginDebugUtilsLabelEXT = nullptr;
PFN_vkCmdEndDebugUtilsLabelEXT Proxy::vkCmdEndDebugUtilsLabelEXT = nullptr;
PFN_vkCmdInsertDebugUtilsLabelEXT Proxy::vkCmdInsertDebugUtilsLabelEXT = nullptr;

// VK_KHR_ray_tracing_pipeline
PFN_vkCmdTraceRaysKHR Proxy::vkCmdTraceRaysKHR = nullptr;
PFN_vkCreateRayTracingPipelinesKHR Proxy::vkCreateRayTracingPipelinesKHR = nullptr;

void Proxy::loadMessengerFunctions(Instance& instance) {
	vkCreateDebugUtilsMessengerEXT = instance.getFunction<PFN_vkCreateDebugUtilsMessengerEXT>("vkCreateDebugUtilsMessengerEXT");
	vkDestroyDebugUtilsMessengerEXT = instance.getFunction<PFN_vkDestroyDebugUtilsMessengerEXT>("vkDestroyDebugUtilsMessengerEXT");
}

void Proxy::loadDebugDeviceFunctions(LogicalDevice& device) {
	vkSetDebugUtilsObjectNameEXT = device.getFunction<PFN_vkSetDebugUtilsObjectNameEXT>("vkSetDebugUtilsObjectNameEXT");
	vkCmdBeginDebugUtilsLabelEXT = device.getFunction<PFN_vkCmdBeginDebugUtilsLabelEXT>("vkCmdBeginDebugUtilsLabelEXT");
	vkCmdEndDebugUtilsLabelEXT = device.getFunction<PFN_vkCmdEndDebugUtilsLabelEXT>("vkCmdEndDebugUtilsLabelEXT");
	vkCmdInsertDebugUtilsLabelEXT = device.getFunction<PFN_vkCmdInsertDebugUtilsLabelEXT>("vkCmdInsertDebugUtilsLabelEXT");
}

void Proxy::loadInstanceFunctions(Instance& instance) {
	vkGetPhysicalDeviceFeatures2KHR = instance.getFunction<PFN_vkGetPhysicalDeviceFeatures2KHR>("vkGetPhysicalDeviceFeatures2KHR");
}

void Proxy::loadDeviceFunctions(LogicalDevice& device) {

}

void Proxy::loadRaytraceFunctions(LogicalDevice& device) {
	vkCmdTraceRaysKHR = device.getFunction<PFN_vkCmdTraceRaysKHR>("vkCmdTraceRaysKHR");
	vkCreateRayTracingPipelinesKHR = device.getFunction<PFN_vkCreateRayTracingPipelinesKHR>("vkCreateRayTracingPipelinesKHR");
}