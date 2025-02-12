
#include "device.hpp"
#include "proxy.hpp"
#include "swapchain.hpp"
#include "render/vulkan/buffer/buffer.hpp"

/*
 * PhysicalDevice
 */

PhysicalDevice::PhysicalDevice(VkPhysicalDevice device)  {
	vkGetPhysicalDeviceProperties(device, &properties);

	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features.pNext = &vk12_features;

	vk12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	vk12_features.pNext = &ray_feature;

	ray_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	ray_feature.pNext = &accel_features;

	accel_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	accel_features.pNext = nullptr;

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

bool PhysicalDevice::hasExtension(const char* name) const {
	uint32_t count = 0;
	vkEnumerateDeviceExtensionProperties(vk_device, nullptr, &count, nullptr);

	std::vector<VkExtensionProperties> extensions {count};
	vkEnumerateDeviceExtensionProperties(vk_device, nullptr, &count, extensions.data());

	for (VkExtensionProperties extension : extensions) {
		if (strcmp(extension.extensionName, name) == 0) {
			return true;
		}
	}

	return false;
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

SwapchainInfo PhysicalDevice::getSwapchainInfo(VkSurfaceKHR surface) {
	return {vk_device, surface};
}

const VkPhysicalDeviceProperties& PhysicalDevice::getProperties() const {
	return properties;
}

const VkPhysicalDeviceLimits& PhysicalDevice::getLimits() const {
	return getProperties().limits;
}

const void* PhysicalDevice::getFeatures(VkStructureType type) const {
	const VkBaseInStructure* current = (VkBaseInStructure*) &features;

	while (true) {
		if (current->sType == type) {
			return current;
		}

		if (current->pNext == nullptr) {
			return nullptr;
		}

		current = current->pNext;
	}
}

/*
 * Logical Device
 */

LogicalDevice::LogicalDevice(VkDevice device) {
	this->vk_device = device;
}

void LogicalDevice::wait() {
	vkDeviceWaitIdle(vk_device);
}

void LogicalDevice::close() {
	vkDestroyDevice(vk_device, nullptr);
}

VkDevice LogicalDevice::getHandle() const {
	return vk_device;
}

Queue LogicalDevice::getQueue(const Family& family) const {
	VkQueue queue;
	vkGetDeviceQueue(vk_device, family.getIndex(), 0, &queue);
	return {queue};
}

VkDeviceAddress LogicalDevice::getAddress(const Buffer& buffer) const {
	VkBufferDeviceAddressInfo info {};
	info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	info.pNext = nullptr;
	info.buffer = buffer.getHandle();

	return Proxy::vkGetBufferDeviceAddressKHR(vk_device, &info);
}