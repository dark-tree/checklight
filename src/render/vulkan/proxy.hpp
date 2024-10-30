#pragma once

#include "external.hpp"

class Instance;
class Device;

struct Proxy {

	static PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
	static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
	static PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR;

	public:

		/**
		 * @brief Load functions only needed in debug mode
		 */
		static void loadDebugFunctions(Instance& instance);

		/**
		 * @brief Load functions through the Vulkan Instance
		 */
		static void loadInstanceFunctions(Instance& instance);

		/**
		 * @brief Load functions through the Vulkan Logical Device
		 */
		static void loadDeviceFunctions(Device& device);

};
