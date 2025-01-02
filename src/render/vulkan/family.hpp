#pragma once

#include "external.hpp"

class Family {

	private:

		VkPhysicalDevice parent;
		VkQueueFamilyProperties properties;
		uint32_t index = 0;

	public:

		Family() = default;
		Family(VkPhysicalDevice device, VkQueueFamilyProperties properties, uint32_t index);

		/**
		 * @brief Check if this queue family has the given usage flags
		 *
		 * @param type combination of one or more VkQueueFlagBits
		 */
		bool hasUsage(VkQueueFlags type) const;

		/**
		 * @brief Check if this queue family can present to the given surface
		 *
		 * @param surface the window surface to check compatibility with
		 */
		bool hasPresentation(VkSurfaceKHR& surface) const;

		/**
		 * @brief Get the queue family identifier (its index in the queue family array)
		 */
		uint32_t getIndex() const;

};
