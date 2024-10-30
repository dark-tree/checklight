
#include "proxy.hpp"
#include "instance.hpp"

PFN_vkCreateDebugUtilsMessengerEXT Proxy::vkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT Proxy::vkDestroyDebugUtilsMessengerEXT = nullptr;
PFN_vkGetPhysicalDeviceFeatures2KHR Proxy::vkGetPhysicalDeviceFeatures2KHR = nullptr;

void Proxy::loadDebugFunctions(Instance& instance) {
	vkCreateDebugUtilsMessengerEXT = instance.getFunction<PFN_vkCreateDebugUtilsMessengerEXT>("vkCreateDebugUtilsMessengerEXT");
	vkDestroyDebugUtilsMessengerEXT = instance.getFunction<PFN_vkDestroyDebugUtilsMessengerEXT>("vkDestroyDebugUtilsMessengerEXT");
}

void Proxy::loadInstanceFunctions(Instance& instance) {
	vkGetPhysicalDeviceFeatures2KHR = instance.getFunction<PFN_vkGetPhysicalDeviceFeatures2KHR>("vkGetPhysicalDeviceFeatures2KHR");
}

void Proxy::loadDeviceFunctions(Device& device) {
	// TODO load device functions
}