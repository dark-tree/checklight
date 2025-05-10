
#include "device.hpp"
#include "proxy.hpp"
#include "swapchain.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/vulkan/raytrace/struct.hpp"

/*
 * PhysicalDevice
 */

PhysicalDevice::PhysicalDevice(VkPhysicalDevice device)  {

	properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
	properties.pNext = &ray_properties;

	ray_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
	ray_properties.pNext = &accel_properties;

	accel_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
	accel_properties.pNext = nullptr;

	Proxy::vkGetPhysicalDeviceProperties2(device, &properties);

	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features.pNext = &vk12_features;

	vk12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	vk12_features.pNext = &ray_feature;

	ray_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	ray_feature.pNext = &accel_features;

	accel_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	accel_features.pNext = nullptr;

	Proxy::vkGetPhysicalDeviceFeatures2(device, &features);
	this->vk_device = device;

	uint32_t count = 0;
	vkEnumerateDeviceExtensionProperties(vk_device, nullptr, &count, nullptr);

	extensions.resize(count);
	vkEnumerateDeviceExtensionProperties(vk_device, nullptr, &count, extensions.data());

}

VkPhysicalDeviceType PhysicalDevice::getType() const {
	return properties.properties.deviceType;
}

const char* PhysicalDevice::getName() const {
	return properties.properties.deviceName;
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
	return properties.properties;
}

const VkPhysicalDeviceLimits& PhysicalDevice::getLimits() const {
	return getProperties().limits;
}

const void* PhysicalDevice::getFeatures(VkStructureType type) const {
	const VkBaseInStructure* current = reinterpret_cast<const VkBaseInStructure*>(&features);

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

const void* PhysicalDevice::getProperties(VkStructureType type) const {
	const VkBaseInStructure* current = reinterpret_cast<const VkBaseInStructure*>(&properties);

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

int PhysicalDevice::getMaxRaytraceRecursionDepth() const {
	return ray_properties.maxRayRecursionDepth;
}

VkSampleCountFlagBits PhysicalDevice::getSampleCount(VkSampleCountFlagBits preferred) const {
	const auto& limits = getLimits();
	VkSampleCountFlags supported = limits.framebufferColorSampleCounts & limits.framebufferDepthSampleCounts;

	// short-circuit in most cases
	if (supported & preferred) {
		return preferred;
	}

	std::array bits = {
		VK_SAMPLE_COUNT_1_BIT,
		VK_SAMPLE_COUNT_2_BIT,
		VK_SAMPLE_COUNT_4_BIT,
		VK_SAMPLE_COUNT_8_BIT,
		VK_SAMPLE_COUNT_16_BIT,
		VK_SAMPLE_COUNT_32_BIT,
		VK_SAMPLE_COUNT_64_BIT,
	};

	auto it = std::find(bits.begin(), bits.end(), preferred);

	if (it == bits.end()) {
		out::error("Failed to find preferred sample count in internal array!");
		return VK_SAMPLE_COUNT_1_BIT;
	}

	while (it != bits.begin()) {
		if (supported & *it) {
			return *it;
		}

		it --;
	}

	out::warn("Multisampling not supported on selected device!");
	return VK_SAMPLE_COUNT_1_BIT;
}

int PhysicalDevice::getScratchBufferAlignment() const {
	return accel_properties.minAccelerationStructureScratchOffsetAlignment;
}


/*
 * Logical Device
 */

LogicalDevice::LogicalDevice(VkDevice device, const std::shared_ptr<PhysicalDevice>& physical)
: vk_device(device), physical(physical) {}

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

	return Proxy::vkGetBufferDeviceAddress(vk_device, &info);
}

VkDeviceAddress LogicalDevice::getAddress(const AccelStruct& structure) const {
	VkAccelerationStructureDeviceAddressInfoKHR info {};
	info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	info.pNext = nullptr;
	info.accelerationStructure = structure.getHandle();

	return Proxy::vkGetAccelerationStructureDeviceAddressKHR(vk_device, &info);
}

std::shared_ptr<PhysicalDevice> LogicalDevice::getPhysical() const {
	return physical;
}
