#pragma once

#include "external.hpp"

class Instance;
class LogicalDevice;

#define DEF_FUNCTION(name) static PFN_##name name

struct Proxy {

		// VK_KHR_get_physical_device_properties2
		DEF_FUNCTION(vkGetPhysicalDeviceFeatures2KHR);

		// VK_EXT_debug_utils
		DEF_FUNCTION(vkCreateDebugUtilsMessengerEXT);
		DEF_FUNCTION(vkDestroyDebugUtilsMessengerEXT);
		DEF_FUNCTION(vkSetDebugUtilsObjectNameEXT);
		DEF_FUNCTION(vkCmdBeginDebugUtilsLabelEXT);
		DEF_FUNCTION(vkCmdEndDebugUtilsLabelEXT);
		DEF_FUNCTION(vkCmdInsertDebugUtilsLabelEXT);

		// VK_KHR_ray_tracing_pipeline
		DEF_FUNCTION(vkCmdTraceRaysKHR);
		DEF_FUNCTION(vkCreateRayTracingPipelinesKHR);

		// VK_KHR_acceleration_structure
		DEF_FUNCTION(vkCreateAccelerationStructureKHR);
		DEF_FUNCTION(vkGetAccelerationStructureBuildSizesKHR);
		DEF_FUNCTION(vkCmdBuildAccelerationStructuresKHR);
		DEF_FUNCTION(vkCmdCopyAccelerationStructureKHR);
		DEF_FUNCTION(vkDestroyAccelerationStructureKHR);
		DEF_FUNCTION(vkGetAccelerationStructureDeviceAddressKHR);

		// VK_EXT_buffer_device_address
		DEF_FUNCTION(vkGetBufferDeviceAddressKHR);

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

#undef DEF_FUNCTION
