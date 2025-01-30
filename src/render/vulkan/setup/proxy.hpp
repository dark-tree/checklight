#pragma once

#include "external.hpp"

class Instance;
class LogicalDevice;

struct Proxy {

	static PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
	static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
	static PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR;
	static PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
	static PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
	static PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
	static PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT;

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
