#pragma once

#include "external.hpp"
#include "device.hpp"

class Instance {

	private:

		VkInstance vk_instance = nullptr;

	public:

		Instance() = default;
		Instance(VkInstance instance);

		template <typename F>
		F getFunction(const char* name) {
			PFN_vkVoidFunction address = vkGetInstanceProcAddr(vk_instance, name);

			if (address == nullptr) {
				printf("ERROR: Instance function '%s' failed to load!\n", name);
			}

			return (F) address;
		}

		/**
		 * @brief Get a list of physical devices,
		 * @note  note that this refers to the Vulkan VKPhysicalDevice not real hardware elements
		 */
		std::vector<PhysicalDevice> getDevices() const;

		/**
		 * @brief Get the underlying vulkan object handle
		 * @note  Try to avoid using this function when possible
		 */
		VkInstance getHandle() const;

		/**
		 * @brief Deletes the underlying vulkan object, must be called after all
		 *        descending objects have been already closed
		 */
		void close();

};
