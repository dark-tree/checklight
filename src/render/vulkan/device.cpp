
#include "device.hpp"
#include "proxy.hpp"

/*
 * PhysicalDevice
 */

PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) {
	vkGetPhysicalDeviceProperties(device, &properties);

	// for future use
	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features.pNext = nullptr;

	Proxy::vkGetPhysicalDeviceFeatures2KHR(device, &features);
	this->vk_device = device;
}

VkPhysicalDeviceType PhysicalDevice::getType() const {
	return properties.deviceType;
}

const char* PhysicalDevice::getName() const {
	return properties.deviceName;
}

bool PhysicalDevice::canUseSurface(VkSurfaceKHR surface) const {
	uint32_t formats, modes;

	// load structure describing capabilities of a surface
	vkGetPhysicalDeviceSurfaceFormatsKHR(vk_device, surface, &formats, nullptr);
	vkGetPhysicalDeviceSurfacePresentModesKHR(vk_device, surface, &modes, nullptr);

	return formats != 0 && modes != 0;
}

VkPhysicalDevice PhysicalDevice::getHandle() const {
	return vk_device;
}

std::vector<Family> PhysicalDevice::getFamilies() const {
	uint32_t count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vk_device, &count, nullptr);

	std::vector<VkQueueFamilyProperties> families {count};
	vkGetPhysicalDeviceQueueFamilyProperties(vk_device, &count, families.data());

	// copy devices into a custom wrapper type
	std::vector<Family> entries;
	entries.reserve(count);

	for (int i = 0; i < (int) families.size(); i ++) {
		entries.emplace_back(vk_device, families[i], i);
	}

	return entries;
}
