#pragma once

#include "external.hpp"

class Instance;
class LogicalDevice;

struct Proxy {

	// VK_KHR_get_physical_device_properties2
	static PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR;

	// VK_EXT_debug_utils
	static PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
	static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
	static PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
	static PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
	static PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
	static PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT;

	// VK_KHR_ray_tracing_pipeline
	static PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
	static PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;

	public:

		/// Load debug messenger functions only needed in debug mode
		static void loadMessengerFunctions(Instance& instance);

		/// Load device functions only needed in debug mode
		static void loadDebugDeviceFunctions(LogicalDevice& device);

		/// Load functions through the Vulkan Instance
		static void loadInstanceFunctions(Instance& instance);

		/// Load functions through the Vulkan Logical Device
		static void loadDeviceFunctions(LogicalDevice& device);

		/// Load all raytracing related functions
		static void loadRaytraceFunctions(LogicalDevice& device);

};
