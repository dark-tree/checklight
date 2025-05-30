
#include "swapchain.hpp"
#include "device.hpp"
#include "render/vulkan/buffer/image.hpp"

#define SWAPCHAIN_EXTENT_AUTO 0xFFFFFFFF

/*
 * Swapchain
 */

Swapchain::Swapchain(VkSwapchainKHR vk_swapchain, VkSurfaceFormatKHR vk_surface_format, VkExtent2D vk_extent, VkDevice vk_device)
: vk_swapchain(vk_swapchain), vk_surface_format(vk_surface_format), vk_extent(vk_extent), vk_device(vk_device) {

	uint32_t count;
	vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &count, nullptr);

	if (count > 0) {
		VkImage* handles = new VkImage[count];
		vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &count, handles);

		for (uint32_t i = 0; i < count; i ++) {
			VkImage vk_image = handles[i];

			Image img {vk_image, vk_surface_format.format, {}};

			images.emplace_back(img);
			views.push_back(ImageView::create(vk_device, img, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT));
		}

		delete[] handles;
	}

}

void Swapchain::close() {
	for (ImageView& view : views) {
		view.close(vk_device);
	}

	vkDestroySwapchainKHR(vk_device, vk_swapchain, nullptr);
}

const std::vector<ImageView>& Swapchain::getViews() const {
	return views;
}

const std::vector<Image>& Swapchain::getImages() const {
	return images;
}

VkExtent2D Swapchain::getExtend() const {
	return vk_extent;
}

bool Swapchain::getNextImage(Semaphore& semaphore, uint32_t* image_index) {
	return vkAcquireNextImageKHR(vk_device, vk_swapchain, UINT64_MAX, semaphore.getHandle(), VK_NULL_HANDLE, image_index) == VK_ERROR_OUT_OF_DATE_KHR;
}

bool Swapchain::present(Queue queue, const Semaphore& await, uint32_t image_index) {

	VkPresentInfoKHR info {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	VkSemaphore semaphore = await.getHandle();
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &semaphore;

	info.swapchainCount = 1;
	info.pSwapchains = &vk_swapchain;
	info.pImageIndices = &image_index;
	info.pResults = nullptr;

	return queue.present(info);
}

/*
 * SwapchainBuilder
 */

SwapchainBuilder::SwapchainBuilder(VkFormat format, VkColorSpaceKHR space, VkExtent2D extent, uint32_t images, VkSurfaceTransformFlagBitsKHR transform)
: extent(extent), images(images), transform(transform) {
	VkSurfaceFormatKHR surface_format;
	surface_format.format = format;
	surface_format.colorSpace = space;

	this->format = surface_format;
}

void SwapchainBuilder::addQueueFamily(Family queue) {
	for (uint32_t family : families) {
		if (queue.getIndex() == family) return;
	}

	families.push_back(queue.getIndex());
}

void SwapchainBuilder::setPresentMode(VkPresentModeKHR mode) {
	this->mode = mode;
}

Swapchain SwapchainBuilder::build(LogicalDevice& device, VkSurfaceKHR vk_surface) {

	VkSwapchainCreateInfoKHR create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = vk_surface;

	// basic creation information
	create_info.minImageCount = images;
	create_info.imageFormat = format.format;
	create_info.imageColorSpace = format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	create_info.presentMode = mode;
	create_info.preTransform = transform;

	// no synchonization needed if we operate on a single queue family
	if (families.size() <= 1) {
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0; // optional
		create_info.pQueueFamilyIndices = nullptr; // optional
	} else {
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = families.size();
		create_info.pQueueFamilyIndices = families.data();
	}

	// how to handle pixels behind the window
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	// ignore pixels that are obscured (for example by a window on top)
	create_info.clipped = VK_TRUE;

	VkSwapchainKHR swapchain;

	if (vkCreateSwapchainKHR(device.getHandle(), &create_info, nullptr, &swapchain) != VK_SUCCESS) {
		FAULT("Failed to create swapchain!");
	}

	return {swapchain, format, extent, device.getHandle()};
}

/*
 * SwapchainInfo
 */

SwapchainInfo::SwapchainInfo(VkPhysicalDevice vk_device, VkSurfaceKHR vk_surface) {
	uint32_t count;

	// load structure describing capabilities of a surface
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_device, vk_surface, &capabilities);

	// load structures describing a supported swapchain format-color space pair
	vkGetPhysicalDeviceSurfaceFormatsKHR(vk_device, vk_surface, &count, nullptr);

	if (count != 0) {
		formats.resize(count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_device, vk_surface, &count, formats.data());
	}

	// load strucutres describing supported presentation modes of a surface
	vkGetPhysicalDeviceSurfacePresentModesKHR(vk_device, vk_surface, &count, nullptr);

	if (count != 0) {
		modes.resize(count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_device, vk_surface, &count, modes.data());
	}
}

const std::vector<VkSurfaceFormatKHR>& SwapchainInfo::getFormats() const {
	return formats;
}

const std::vector<VkPresentModeKHR>& SwapchainInfo::getModes() const {
	return modes;
}

VkExtent2D SwapchainInfo::getExtent(Window& window) const {

	// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSurfaceCapabilitiesKHR.html
	// check agains the special value specified in the spec
	if (capabilities.currentExtent.width == SWAPCHAIN_EXTENT_AUTO && capabilities.currentExtent.height == SWAPCHAIN_EXTENT_AUTO) {
		return capabilities.currentExtent;
	}

	int width, height;
	window.getFramebufferSize(&width, &height);

	VkExtent2D extent {};
	extent.width = std::clamp((uint32_t) width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	extent.height = std::clamp((uint32_t) height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	return extent;

}

uint32_t SwapchainInfo::getImageCount(uint32_t images) const {

	// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSurfaceCapabilitiesKHR.html
	// is maxImageCount is 0 that means that there is no limit (other than free memory)
	const uint32_t min_images = capabilities.minImageCount;
	const uint32_t max_images = capabilities.maxImageCount == 0 ? 0xffff : capabilities.maxImageCount;

	return std::clamp(images, min_images, max_images);

}

VkSurfaceTransformFlagBitsKHR SwapchainInfo::getTransform() const {
	return capabilities.currentTransform;
}

bool SwapchainInfo::isFormatSupported(VkFormat expected_format, VkColorSpaceKHR expected_space) {
	for (auto format : formats) {
		if (format.format == expected_format && format.colorSpace == expected_space) {
			return true;
		}
	}

	return false;
}

