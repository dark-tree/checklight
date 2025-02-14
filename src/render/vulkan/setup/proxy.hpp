#pragma once

#include "external.hpp"

class Instance;
class LogicalDevice;

#define DEF_FUNCTION(name) static PFN_##name name

struct Proxy {

		// Vulkan >1.0
		DEF_FUNCTION(vkGetBufferDeviceAddress);
		DEF_FUNCTION(vkGetPhysicalDeviceFeatures2);
		DEF_FUNCTION(vkGetPhysicalDeviceProperties2);

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
		DEF_FUNCTION(vkGetRayTracingShaderGroupHandlesKHR);

		// VK_KHR_acceleration_structure
		DEF_FUNCTION(vkCreateAccelerationStructureKHR);
		DEF_FUNCTION(vkGetAccelerationStructureBuildSizesKHR);
		DEF_FUNCTION(vkCmdBuildAccelerationStructuresKHR);
		DEF_FUNCTION(vkCmdCopyAccelerationStructureKHR);
		DEF_FUNCTION(vkDestroyAccelerationStructureKHR);
		DEF_FUNCTION(vkGetAccelerationStructureDeviceAddressKHR);
		DEF_FUNCTION(vkCmdWriteAccelerationStructuresPropertiesKHR);

	public:

		/// Load debug messenger functions only needed in debug mode
		static void loadMessengerFunctions(Instance& instance);

		/// Load device functions only needed in debug mode
		static void loadDebugDeviceFunctions(LogicalDevice& device);

		/// Load functions through the Vulkan Instance
		static void loadInstanceFunctions(Instance& instance);

		/// Load functions through the Vulkan Logical Device
		static void loadDeviceFunctions(LogicalDevice& device);

};

#undef DEF_FUNCTION
