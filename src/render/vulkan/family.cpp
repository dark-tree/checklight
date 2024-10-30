
#include "family.hpp"

/*
 * Family
 */

Family::Family(VkPhysicalDevice parent, VkQueueFamilyProperties properties, int index) {
	this->parent = parent;
	this->properties = properties;
	this->index = index;
}

bool Family::hasUsage(VkQueueFlags type) {
	return properties.queueFlags & type;
}

bool Family::hasPresentation(VkSurfaceKHR& surface) {
	VkBool32 supported = true;
	vkGetPhysicalDeviceSurfaceSupportKHR(parent, index, surface, &supported);
	return supported;
}

int Family::getIndex() {
	return index;
}