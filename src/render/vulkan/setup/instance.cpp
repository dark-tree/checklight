
#include "instance.hpp"

/*
 * Instance
 */

Instance::Instance(VkInstance instance) {
	this->vk_instance = instance;
}

std::vector<std::unique_ptr<PhysicalDevice>> Instance::getDevices() const {
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(vk_instance, &count, nullptr);

	std::vector<VkPhysicalDevice> devices {count};
	vkEnumeratePhysicalDevices(vk_instance, &count, devices.data());

	// copy devices into a custom wrapper type
	std::vector<std::unique_ptr<PhysicalDevice>> entries;
	entries.reserve(count);

	for (VkPhysicalDevice device : devices) {
		entries.emplace_back(std::make_unique<PhysicalDevice>(device));
	}

	return entries;
}

VkInstance Instance::getHandle() const {
	return vk_instance;
}

void Instance::close() {
	if (vk_instance) vkDestroyInstance(vk_instance, nullptr);
}