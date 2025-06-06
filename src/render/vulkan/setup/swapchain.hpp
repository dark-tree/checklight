#pragma once

#include "external.hpp"
#include "family.hpp"
#include "render/window.hpp"
#include "render/vulkan/buffer/image.hpp"
#include "render/vulkan/sync/semaphore.hpp"

class LogicalDevice;

class Swapchain {

	private:

		VkSwapchainKHR vk_swapchain;
		VkSurfaceFormatKHR vk_surface_format;
		VkExtent2D vk_extent;
		VkDevice vk_device;

		std::vector<Image> images;
		std::vector<ImageView> views;

	public:

		Swapchain() = default;
		Swapchain(VkSwapchainKHR vk_swapchain, VkSurfaceFormatKHR vk_surface_format, VkExtent2D vk_extent, VkDevice vk_device);

		void close();

		/// Get a list of views into the screen color buffer images
		const std::vector<ImageView>& getViews() const;

		/// Get a list of window images returned by the driver
		const std::vector<Image>& getImages() const;

		/// Get swapchain (screen) dimensions
		VkExtent2D getExtend() const;

		/// Acquire next, currently unused, screen color buffer
		bool getNextImage(Semaphore& semaphore, uint32_t* image_index);

		/// Push image to be shown on screen in line with specified refresh policy
		bool present(Queue queue, const Semaphore& await, uint32_t image_index);

};

class SwapchainBuilder {

	private:

		const VkExtent2D extent;
		const uint32_t images;
		const VkSurfaceTransformFlagBitsKHR transform;

		VkPresentModeKHR mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		VkSurfaceFormatKHR format;
		std::vector<uint32_t> families;

	public:

		SwapchainBuilder(VkFormat format, VkColorSpaceKHR space, VkExtent2D extent, uint32_t images, VkSurfaceTransformFlagBitsKHR transform);

		/// Add queue family to the swapchain, if more than one family is given synchronization will be enabled
		void addQueueFamily(Family queue);

		/// Set the image swapping mode, set to VK_PRESENT_MODE_IMMEDIATE_KHR to disable "vsync"
		void setPresentMode(VkPresentModeKHR mode);

		/// Create new Swapchain with the configured settings
		Swapchain build(LogicalDevice& device, VkSurfaceKHR vk_surface);

};

class SwapchainInfo {

	private:

		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> modes;

	public:

		SwapchainInfo() = default;
		SwapchainInfo(VkPhysicalDevice vk_device, VkSurfaceKHR vk_surface);

		/// Get supported format list
		const std::vector<VkSurfaceFormatKHR>& getFormats() const;

		/// Get supported mode list
		const std::vector<VkPresentModeKHR>& getModes() const;

		/// Get extend for a given window, supported by the swapchain
		VkExtent2D getExtent(Window& window) const;

		/// Clamp desired image number into a supported range
		uint32_t getImageCount(uint32_t images) const;

		/// Get the surface transform of the swapchain
		VkSurfaceTransformFlagBitsKHR getTransform() const;

		/// Checks if this swapchain supports the given surface format
		bool isFormatSupported(VkFormat format, VkColorSpaceKHR space);
};
