#pragma once

#include "external.hpp"
#include "family.hpp"
#include "queue.hpp"

class PhysicalDevice {

	private:

		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures2KHR features;
		VkPhysicalDevice vk_device;

	public:

		PhysicalDevice() = default;
		PhysicalDevice(VkPhysicalDevice device);

		/**
		 * @brief Get the type of the physical device,
		 *        CPU, GPU, etc.
		 */
		VkPhysicalDeviceType getType() const;

		/**
		 * @brief Get the name of the physical device,
		 *        as reported by the driver.
		 */
		const char* getName() const;

		/**
		 * @brief Check if the device's swap chain is compatible with our window surface,
		 *        we will consider it compatible if at least one image format and presentation mode match
		 *
		 * @param surface the window surface to check compatibility with
		 */
		bool canUseSurface(VkSurfaceKHR surface) const;

		/**
		 * @brief Get the underlying vulkan object handle
		 * @note  Try to avoid using this function when possible
		 */
		VkPhysicalDevice getHandle() const;

		/**
		 * @brief Get the list of extensions supported by this device
		 */
		bool hasExtension(const char* name) const;

		/**
		 * @brief Get the list of queue families supported by this device
		 */
		std::vector<Family> getFamilies() const;

};

class LogicalDevice {

	private:

		VkDevice vk_device = nullptr;

	public:

		LogicalDevice() = default;
		LogicalDevice(VkDevice device);

		template <typename F>
		F getFunction(const char* name) {
			PFN_vkVoidFunction address = vkGetDeviceProcAddr(vk_device, name);

			if (address == nullptr) {
				printf("ERROR: Device function '%s' failed to load!\n", name);
			}

			return (F) address;
		}

		/**
		 * @brief Deletes the underlying vulkan object, must be called after all
		 *        descending objects have been already closed
		 */
		void close();

		/**
		 * @brief Get the underlying vulkan object handle
		 * @note  Try to avoid using this function when possible
		 */
		VkDevice getHandle() const;

		/**
		 * @brief Get the queue corresponding to the given
		 *        Family used during device creation
		 */
		Queue getQueue(const Family& family) const;

};