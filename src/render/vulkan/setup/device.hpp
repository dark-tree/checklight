#pragma once

#include "external.hpp"
#include "family.hpp"
#include "queue.hpp"
#include "shared/logger.hpp"

class Buffer;
class SwapchainInfo;
class AccelStruct;

/**
 * Represents the vulkan driver,
 * this often corresponds with actual hardware but can include other drivers (e.g. software renderers)
 */
class PhysicalDevice {

	private:

		// features
		VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray_feature {};
		VkPhysicalDeviceVulkan12Features vk12_features {};
		VkPhysicalDeviceAccelerationStructureFeaturesKHR accel_features {};

		// properties
		VkPhysicalDeviceRayTracingPipelinePropertiesKHR ray_properties;
		VkPhysicalDeviceAccelerationStructurePropertiesKHR accel_properties;

		VkPhysicalDeviceProperties2KHR properties;
		VkPhysicalDeviceFeatures2KHR features;
		VkPhysicalDevice vk_device;
		std::vector<VkExtensionProperties> extensions;

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
		 * Check if the device's swap chain is compatible with our window surface,
		 * we will consider it compatible if at least one image format and presentation mode match
		 *
		 * @param[in] surface the window surface to check compatibility with
		 */
		bool canUseSurface(VkSurfaceKHR surface) const;

		/**
		 * Get the underlying vulkan object handle
		 */
		VkPhysicalDevice getHandle() const;

		/**
		 * Get the list of extensions supported by this device
		 */
		bool hasExtension(const char* name) const;

		/**
		 * Get the list of queue families supported by this device
		 */
		std::vector<Family> getFamilies() const;

		/**
		 * Get information about supported swapchain configurations
		 */
		SwapchainInfo getSwapchainInfo(VkSurfaceKHR surface);

		/**
		 * Get the cached device properties
		 */
		const VkPhysicalDeviceProperties& getProperties() const;

		/**
		 * Get the cached device limits
		 */
		const VkPhysicalDeviceLimits& getLimits() const;

		/**
		 * Get the cached device feature chain entry
		 */
		const void* getFeatures(VkStructureType type) const;

		/**
		 * Get the cached device properties chain entry
		 */
		const void* getProperties(VkStructureType type) const;

		/**
		 * Get the device's maximal ray invocation recursion
		 * depth, learn more in @see RaytracePipelineBuilder::withRecursionDepth()
		 */
		int getMaxRaytraceRecursionDepth() const;

		/**
		 * Get the supported sampling count that is the closest to the preferred value
		 */
		VkSampleCountFlagBits getSampleCount(VkSampleCountFlagBits preferred) const;

		/**
		 * Get alignment requirement of the Acceleration Structure Scratch buffer
		 */
		int getScratchBufferAlignment() const;

};

/**
 * Represents the vulkan driver instance (connection)
 */
class LogicalDevice {

	private:

		VkDevice vk_device = VK_NULL_HANDLE;
		std::shared_ptr<PhysicalDevice> physical;

	public:

		LogicalDevice() = default;
		LogicalDevice(VkDevice device, const std::shared_ptr<PhysicalDevice>& physical);

		template <typename F>
		F getFunction(const char* name) {
			PFN_vkVoidFunction address = vkGetDeviceProcAddr(vk_device, name);

			if (address == nullptr) {
				out::error("Device function '%s' failed to load!", name);
			}

			return (F) address;
		}

		/**
		 * Waits (blocks) for the Device (GPU) to finish all pending work
		 */
		 void wait();

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

		/**
		 * @brief Get device address of this buffer
		 * @note  Device addresses are a type of a universal pointer
		 */
		VkDeviceAddress getAddress(const Buffer& buffer) const;

		/**
		 * @brief Get device address of this acceleration structure
		 * @note  Device addresses are a type of a universal pointer
		 */
		VkDeviceAddress getAddress(const AccelStruct& structure) const;

		/**
		 * @brief Get a reference to the physical device from which
		 *        this logical device was created
		 */
		std::shared_ptr<PhysicalDevice> getPhysical() const;

};
