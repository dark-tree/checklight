
#include "renderer.hpp"
#include "const.hpp"

#include "vulkan/device.hpp"

/*
 * Renderer
 */

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* user) {
	return ((Renderer*) user)->onMessage(severity, data);
}

VkBool32 Renderer::onMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity, const VkDebugUtilsMessengerCallbackDataEXT* data) {
	if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		printf("ERROR: %s\n", data->pMessage);
		return VK_FALSE;
	}

	if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		printf("WARN: %s\n", data->pMessage);
		return VK_FALSE;
	}

	return VK_FALSE;
}

void Renderer::createInstance(ApplicationParameters& parameters, bool debug) {
	this->instance.close();
	this->messenger = nullptr;

	auto name = parameters.getName();

	// optional info about our application
	VkApplicationInfo app_info {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = name.c_str();
	app_info.applicationVersion = VK_MAKE_VERSION(parameters.major, parameters.minor, parameters.patch);
	app_info.pEngineName = ENGINE_NAME;
	app_info.engineVersion = ENGINE_VERSION;
	app_info.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> extension = windows.getRequiredExtensions();
	std::vector<const char*> layers;

	// needed for the vkGetPhysicalDeviceFeatures2KHR which we will need to
	// use any extensions or new vulkan features
	extension.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	if (debug) {
		layers.push_back("VK_LAYER_KHRONOS_validation");
		extension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		printf("INFO: Starting renderer in debug mode, performance will be affected!\n");
	}

	// configure the messenger, this will later be used during instance creation and right after it
	VkDebugUtilsMessengerCreateInfoEXT messenger_info {};
	messenger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	messenger_info.pfnUserCallback = VulkanMessageCallback;
	messenger_info.pUserData = this;

	// information required for creating an instance
	VkInstanceCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	create_info.enabledExtensionCount = extension.size();
	create_info.ppEnabledExtensionNames = extension.data();
	create_info.enabledLayerCount = layers.size();
	create_info.ppEnabledLayerNames = layers.data();

	// attach debug messenger during instance creation
	if (debug) {
		create_info.pNext = &messenger_info;
	}

	VkInstance vk_instance;

	if (vkCreateInstance(&create_info, nullptr, &vk_instance) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create Vulkan instance!"};
	}

	// store for later use
	this->instance = vk_instance;

	// attach debug messenger for all calls after instance creation
	if (debug) {
		// load functions from debug utils extension
		Proxy::loadDebugFunctions(instance);

		// create the messenger
		if (Proxy::vkCreateDebugUtilsMessengerEXT(vk_instance, &messenger_info, nullptr, &messenger) != VK_SUCCESS) {
			throw std::runtime_error {"Failed to create debug messenger!"};
		}
	}

	// load all other instance extension functions
	Proxy::loadInstanceFunctions(instance);
}

void Renderer::pickDevice() {

	auto devices = instance.getDevices();
	printf("INFO: Detected %d physical devices\n", (int) devices.size());

	for (PhysicalDevice device : devices) {

		// we need the device to be able to render to our window
		if (!device.canUseSurface(surface)) {
			continue;
		}

		// check for support of required extensions
		if (!device.hasExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
			continue;
		}

		// find a queue family of our liking
		for (Family queue_family : device.getFamilies()) {

			// needs to be able to present to our window...
			if (!queue_family.hasPresentation(surface)) {
				continue;
			}

			// we will do all vulkan calls on a single queue for simplicity
			if (!queue_family.hasUsage(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT)) {
				continue;
			}

			// we found the one, continue with this device and family
			createDevice(device, queue_family);
			return;
		}

	}

	throw std::runtime_error {"No device could have been selected!"};
}

void Renderer::createDevice(PhysicalDevice physical, Family queue_family) {
	printf("INFO: Selected '%s' (queue #%d)\n", physical.getName(), queue_family.getIndex());

	// list device extensions we need
	std::vector<const char*> extensions;
	extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// we use only one queue at this time
	const float priority = 1.0f;
	VkDeviceQueueCreateInfo queue_info {};
	queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.pNext = nullptr;
	queue_info.queueFamilyIndex = queue_family.getIndex();
	queue_info.queueCount = 1;
	queue_info.pQueuePriorities = &priority;

	// features we want to enable
	VkPhysicalDeviceFeatures2KHR features {};
	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

	// we will now connect with the selected driver
	VkDeviceCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pNext = &features;
	create_info.pQueueCreateInfos = &queue_info;
	create_info.queueCreateInfoCount = 1;
	create_info.pEnabledFeatures = nullptr;
	create_info.enabledExtensionCount = extensions.size();
	create_info.ppEnabledExtensionNames = extensions.data();

	// deprecated and ignored
	create_info.enabledLayerCount = 0;

	VkDevice vk_device;

	if (vkCreateDevice(physical.getHandle(), &create_info, nullptr, &vk_device) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create logical device!"};
	}

	// load all device functions
	this->physical = physical;
	this->device = vk_device;
	this->family = queue_family;

	Proxy::loadDeviceFunctions(this->device);

}

void Renderer::createSwapchain() {

	SwapchainInfo info = physical.getSwapchainInfo(this->surface);
	auto extent = info.getExtent(*window);
	auto images = info.getImageCount(4);
	auto transform = info.getTransform();

	const VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;
	const VkColorSpaceKHR space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	if (!info.isFormatSupported(format, space)) {
		throw std::runtime_error {"The preferred surface format is not supported!"};
	}

	SwapchainBuilder builder {format, space, extent, images, transform};
	builder.setPresentMode(VK_PRESENT_MODE_IMMEDIATE_KHR);
	builder.addQueueFamily(family);

	this->swapchain = builder.build(device, surface);
	printf("INFO: Swapchain ready\n");

}

void Renderer::lateClose() {
	swapchain.close();
}

void Renderer::lateInit() {
	createSwapchain();
}

Renderer::Renderer(ApplicationParameters& parameters)
: windows(), window(windows.open(parameters.width, parameters.height, parameters.getTitle())) {

	// early init
	createInstance(parameters, true);
	glfwCreateWindowSurface(instance.getHandle(), window->getHandle(), nullptr, &surface);
	pickDevice();

	// family is selected and loaded as part of device picking
	queue = device.getQueue(family);

	// create command buffer pools for our family
	transient_pool = CommandPool::create(device, family, true);
	graphics_pool = CommandPool::create(device, family, false);

	// begin late initialization
	lateInit();

}

Renderer::~Renderer() {

	// late close is actually first :)
	lateClose();

	transient_pool.close();
	graphics_pool.close();

	// destroy messenger only if it was created before
	if (messenger) {
		Proxy::vkDestroyDebugUtilsMessengerEXT(instance.getHandle(), messenger, nullptr);
	}

	vkDestroySurfaceKHR(instance.getHandle(), surface, nullptr);

	// It's important to maintain the correct order
	device.close();
	instance.close();

	printf("INFO: Renderer deinitialized, goodbye!\n");

}

void Renderer::reload() {
	lateClose();
	lateInit();
}

Window& Renderer::getWindow() const {
	return *window;
}