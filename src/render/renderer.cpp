
#include "renderer.hpp"
#include "const.hpp"

#include "render/vulkan/setup/device.hpp"
#include "render/vulkan/buffer/texture.hpp"
#include "render/vulkan/pass/render.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/vulkan/setup/debug.hpp"
#include "render/api/vertex.hpp"
#include "render/api/model.hpp"
#include "render/vulkan/shader/group.hpp"

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

void Renderer::createInstance(ApplicationParameters& parameters) {
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
	app_info.apiVersion = VK_API_VERSION_1_2;

	std::vector<const char*> extension = windows.getRequiredExtensions();
	std::vector<const char*> layers;

	#if ENGINE_DEBUG
		layers.push_back("VK_LAYER_KHRONOS_validation");
		extension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		printf("INFO: Starting renderer in debug mode, performance will be affected!\n");
	#endif

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
	#if ENGINE_DEBUG
		create_info.pNext = &messenger_info;
	#endif

	VkInstance vk_instance;

	if (vkCreateInstance(&create_info, nullptr, &vk_instance) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create Vulkan instance!"};
	}

	// store for later use
	this->instance = vk_instance;

	// attach debug messenger for all calls after instance creation
	#if ENGINE_DEBUG
		// load functions from debug utils extension
		Proxy::loadMessengerFunctions(instance);

		// create the messenger
		if (Proxy::vkCreateDebugUtilsMessengerEXT(vk_instance, &messenger_info, nullptr, &messenger) != VK_SUCCESS) {
			throw std::runtime_error {"Failed to create debug messenger!"};
		}
	#endif

	// load all other instance extension functions
	Proxy::loadInstanceFunctions(instance);
}

void Renderer::pickDevice() {

	auto devices = instance.getDevices();
	printf("INFO: Detected %d physical devices\n", (int) devices.size());

	std::vector<const char*> required_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
	};

	std::vector<const char*> optional_extensions = {
	};

	std::vector<std::string> debug;

	for (auto& device : devices) {

		printf("INFO: Checking device '%s'...\n", device->getName());
		bool fail = false;

		// we need the device to be able to render to our window
		if (!device->canUseSurface(surface)) {
			printf(" * Can't use window's surface!\n");
			fail = true;
		}

		// check for support of required extensions
		for (auto name : required_extensions) {
			if (!device->hasExtension(name)) {
				printf(" * Missing required extension '%s'!\n", name);
				fail = true;
			}
		}

		auto* features_base = (const VkPhysicalDeviceFeatures2*) device->getFeatures(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
		auto* features_vk12 = (const VkPhysicalDeviceVulkan12Features*) device->getFeatures(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES);
		auto* features_ray = (const VkPhysicalDeviceRayTracingPipelineFeaturesKHR*) device->getFeatures(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR);
		auto* features_accel = (const VkPhysicalDeviceAccelerationStructureFeaturesKHR*) device->getFeatures(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR);

		if (!features_vk12->bufferDeviceAddress) {
			printf(" * Feature 'buffer device address' unsupported!\n");
			fail = true;
		}

		if (!features_ray->rayTracingPipeline) {
			printf(" * Feature 'ray tracing pipeline' unsupported!\n");
			fail = true;
		}

		if (!features_accel->accelerationStructure) {
			printf(" * Feature 'acceleration structure' unsupported!\n");
			fail = true;
		}

		if (!features_vk12->scalarBlockLayout) {
			printf(" * Feature 'scalar block layout' unsupported!\n");
			fail = true;
		}

		if (!features_vk12->storageBuffer8BitAccess) {
			printf(" * Feature 'storage buffer 8bit access' unsupported!\n");
			fail = true;
		}

		if (!features_vk12->shaderInt8) {
			printf(" * Feature 'shader int8_t' unsupported!\n");
			fail = true;
		}

		if (!features_vk12->runtimeDescriptorArray) {
			printf(" * Feature 'runtime descriptor array' unsupported!\n");
			fail = true;
		}

		if (!features_vk12->shaderSampledImageArrayNonUniformIndexing) {
			printf(" * Feature 'shader sampled image array non uniform indexing' unsupported!\n");
			fail = true;
		}

		if (!features_base->features.shaderInt64) {
			printf(" * Feature 'shader uint64_t' unsupported!\n");
			fail = true;
		}

		if (fail) {
			continue;
		}

		// find a queue family of our liking
		for (Family queue_family : device->getFamilies()) {

			// needs to be able to present to our window...
			if (!queue_family.hasPresentation(surface)) {
				continue;
			}

			// we will do all vulkan calls on a single queue for simplicity
			if (!queue_family.hasUsage(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT)) {
				continue;
			}

			// we found the one, continue with this device and family
			createDevice(std::move(device), queue_family, required_extensions, optional_extensions);
			return;
		}

		printf(" * No valid queue!\n");

	}

	throw std::runtime_error {"No device could have been selected!"};
}

void Renderer::createDevice(std::shared_ptr<PhysicalDevice> physical, Family queue_family, std::vector<const char*>& extensions, std::vector<const char*>& optionals) {
	printf("INFO: Selected '%s' (queue #%d)\n", physical->getName(), queue_family.getIndex());

	// enable supported optional extensions
	for (auto name : optionals) {
		if (!physical->hasExtension(name)) {
			printf("WARN: Missing optional extension '%s'!\n", name);
			continue;
		}

		extensions.emplace_back(name);
	}

	// we use only one queue at this time
	const float priority = 1.0f;
	VkDeviceQueueCreateInfo queue_info {};
	queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.pNext = nullptr;
	queue_info.queueFamilyIndex = queue_family.getIndex();
	queue_info.queueCount = 1;
	queue_info.pQueuePriorities = &priority;

	// AccelStruct features
	VkPhysicalDeviceAccelerationStructureFeaturesKHR feature_accel {};
	feature_accel.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	feature_accel.pNext = nullptr;
	feature_accel.accelerationStructure = true;

	// Raytracing features
	VkPhysicalDeviceRayTracingPipelineFeaturesKHR features_ray {};
	features_ray.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	features_ray.pNext = &feature_accel;
	features_ray.rayTracingPipeline = true;

	// Vulkan 1.2 features
	VkPhysicalDeviceVulkan12Features features_vk12 {};
	features_vk12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	features_vk12.pNext = &features_ray;
	features_vk12.bufferDeviceAddress = true; // need for raytracing, allow creating the funny universal pointers
	features_vk12.scalarBlockLayout = true; // needed for the shader
	features_vk12.storageBuffer8BitAccess = true; // needed for the shader RGBA block
	features_vk12.shaderInt8 = true; // needed for the shader RGBA block
	features_vk12.runtimeDescriptorArray = true; // needed for the shader
	features_vk12.shaderSampledImageArrayNonUniformIndexing = true; // needed for the shader

	// Basic device features
	VkPhysicalDeviceFeatures2KHR features {};
	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features.pNext = &features_vk12;
	features.features.shaderInt64 = true; // needed for the shader

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

	if (vkCreateDevice(physical->getHandle(), &create_info, nullptr, &vk_device) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create logical device!"};
	}

	// load all device functions
	this->device = {vk_device, physical};
	this->family = queue_family;
	this->physical = std::move(physical);

	#if ENGINE_DEBUG
		Proxy::loadDebugDeviceFunctions(this->device);
	#endif

	Proxy::loadDeviceFunctions(this->device);
}

void Renderer::createSwapchain() {

	SwapchainInfo info = physical->getSwapchainInfo(this->surface);
	auto extent = info.getExtent(*window);
	auto images = info.getImageCount(4);
	auto transform = info.getTransform();

	const VkFormat format = attachment_color.getFormat();
	const VkColorSpaceKHR space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	if (!info.isFormatSupported(format, space)) {
		throw std::runtime_error {"The preferred surface format is not supported!"};
	}

	SwapchainBuilder builder {format, space, extent, images, transform};
	builder.setPresentMode(VK_PRESENT_MODE_IMMEDIATE_KHR);
	builder.addQueueFamily(family);

	this->swapchain = builder.build(device, surface);
	this->immediate.setResolution(width(), height());

	// allocate all attachments (except for color)
	attachment_depth.allocate(device, extent.width, extent.height, allocator);
	attachment_albedo.allocate(device, extent.width, extent.height, allocator);
	attachment_illumination.allocate(device, extent.width, extent.height, allocator);
	attachment_prev_illumination.allocate(device, extent.width, extent.height, allocator);
	attachment_normal.allocate(device, extent.width, extent.height, allocator);
	attachment_prev_normal.allocate(device, extent.width, extent.height, allocator);
	attachment_illum_transport.allocate(device, extent.width, extent.height, allocator);
	attachment_soild_illumination.allocate(device, extent.width, extent.height, allocator);
	attachment_world_position.allocate(device, extent.width, extent.height, allocator);
	attachment_prev_world_position.allocate(device, extent.width, extent.height, allocator);

	// create framebuffers
	pass_immediate.prepareFramebuffers(swapchain);
	pass_compose.prepareFramebuffers(swapchain);
	pass_denoise.prepareFramebuffers(swapchain);
	pass_denoise2.prepareFramebuffers(swapchain);

	printf("INFO: Swapchain ready\n");

}

void Renderer::createShaders() {

	shader_screen_vertex = Shader::loadFromFile(device, "screen.vert", VK_SHADER_STAGE_VERTEX_BIT);
	shader_world_vertex = Shader::loadFromFile(device, "world.vert", VK_SHADER_STAGE_VERTEX_BIT);
	shader_atlas_fragment = Shader::loadFromFile(device, "atlas.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	shader_text_fragment = Shader::loadFromFile(device, "text.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	shader_trace_gen = Shader::loadFromFile(device, "trace.rgen", VK_SHADER_STAGE_RAYGEN_BIT_KHR);
	shader_trace_miss = Shader::loadFromFile(device, "trace.rmiss", VK_SHADER_STAGE_MISS_BIT_KHR);
	shader_trace_shadow_miss = Shader::loadFromFile(device, "shadow.rmiss", VK_SHADER_STAGE_MISS_BIT_KHR);
	shader_trace_hit = Shader::loadFromFile(device, "trace.rchit", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	shader_blit_vertex = Shader::loadFromFile(device, "blit.vert", VK_SHADER_STAGE_VERTEX_BIT);
	shader_blit_fragment = Shader::loadFromFile(device, "blit.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	shader_denoise_fragment = Shader::loadFromFile(device, "denoise.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	shader_denoise2_fragment = Shader::loadFromFile(device, "denoise2.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

}

void Renderer::createAttachments() {

	/*
	 * When adding a new attachment create it here, then invoke .allocate()
	 * on in the createSwapchain in the designated location, finally invoke close()
	 * on it at the start of lateClose().
	 */

	// this attachment is special - we will never allocate it
	attachment_color = TextureBuilder::begin()
		.setFormat(surface_format)
		.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
		.setClearColor(3 / 255.0f, 169 / 255.0f, 252 / 255.0f, 1.0f)
		.setDebugName("Color")
		.createAttachment();

	attachment_depth = TextureBuilder::begin()
		.setFormat(VK_FORMAT_D32_SFLOAT)
		.setAspect(VK_IMAGE_ASPECT_DEPTH_BIT)
		.setClearDepth(1.0f)
		.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		.setDebugName("Depth")
		.createAttachment();

	attachment_albedo = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R32G32B32A32_SFLOAT)
		.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
		.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
		.setDebugName("Albedo")
		.createAttachment();

	attachment_illumination = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
		.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
		.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		.setDebugName("Illumination")
		.createAttachment();

	attachment_prev_illumination = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
		.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
		.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		.setDebugName("Prev Illumination")
		.createAttachment();

	attachment_normal = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
		.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
		.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
		.setDebugName("Normal")
		.createAttachment();

	attachment_prev_normal = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
		.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
		.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		.setDebugName("Prev Normal")
		.createAttachment();

	attachment_illum_transport = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
		.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
		.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		.setDebugName("Illum transport")
		.createAttachment();

	attachment_soild_illumination = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
		.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
		.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
		.setDebugName("Solid Illumination")
		.createAttachment();

	attachment_world_position = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R32G32B32A32_SFLOAT)
		.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
		.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
		.setDebugName("World Position")
		.createAttachment();

	attachment_prev_world_position = TextureBuilder::begin()
		.setFormat(VK_FORMAT_R32G32B32A32_SFLOAT)
		.setAspect(VK_IMAGE_ASPECT_COLOR_BIT)
		.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		.setDebugName("Prev World Position")
		.createAttachment();

	// very important UwU
	attachment_color.markSwapchainBacked();

}

void Renderer::createRenderPasses() {

	/*
	 * If you are adding a new RenderPass remember to invoke
	 * prepareFramebuffers() on it at the end of createSwapchain()!
	 */

	{ // immediate 2d/3d pass

		RenderPassBuilder builder;

		Attachment::Ref color = builder.addAttachment(attachment_color)
			.begin(ColorOp::LOAD, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.end(ColorOp::STORE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
			.next();

		Attachment::Ref depth = builder.addAttachment(attachment_depth)
			.begin(ColorOp::LOAD, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			.end(ColorOp::STORE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			.next();

		builder.addDependency()
			.first(VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0)
			.then(0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
			.next();

		builder.addDependency(VK_DEPENDENCY_BY_REGION_BIT)
			.first(0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
			.then(VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_MEMORY_READ_BIT)
			.next();

		builder.addSubpass()
			.addOutput(color, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.addDepth(depth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			.next();

		pass_immediate = builder.build(device, "Immediate");

	}

	{ // compose 2d pass

		RenderPassBuilder builder;

		Attachment::Ref color = builder.addAttachment(attachment_color)
			.begin(ColorOp::CLEAR, VK_IMAGE_LAYOUT_UNDEFINED)
			.end(ColorOp::STORE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.next();

		Attachment::Ref depth = builder.addAttachment(attachment_depth)
			.begin(ColorOp::CLEAR, VK_IMAGE_LAYOUT_UNDEFINED)
			.end(ColorOp::STORE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			.next();

		Attachment::Ref prev_normal = builder.addAttachment(attachment_prev_normal)
			.begin(ColorOp::LOAD, VK_IMAGE_LAYOUT_GENERAL)
			.end(ColorOp::STORE, VK_IMAGE_LAYOUT_GENERAL)
			.next();

		Attachment::Ref prev_pos = builder.addAttachment(attachment_prev_world_position)
			.begin(ColorOp::LOAD, VK_IMAGE_LAYOUT_GENERAL)
			.end(ColorOp::STORE, VK_IMAGE_LAYOUT_GENERAL)
			.next();

		builder.addDependency()
			.first(VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0)
			.then(0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
			.next();

		builder.addDependency(VK_DEPENDENCY_BY_REGION_BIT)
			.first(0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
			.then(VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_MEMORY_READ_BIT)
			.next();

		builder.addSubpass()
			.addOutput(color, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.addOutput(prev_normal, VK_IMAGE_LAYOUT_GENERAL)
			.addOutput(prev_pos, VK_IMAGE_LAYOUT_GENERAL)
			.addDepth(depth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			.next();

		pass_compose = builder.build(device, "Compose");

	}

	{ // denoise 2d pass

		RenderPassBuilder builder;

		Attachment::Ref illum = builder.addAttachment(attachment_illum_transport)
			.begin(ColorOp::CLEAR, VK_IMAGE_LAYOUT_GENERAL)
			.end(ColorOp::STORE, VK_IMAGE_LAYOUT_GENERAL)
			.next();

		Attachment::Ref illum_prev = builder.addAttachment(attachment_prev_illumination)
			.begin(ColorOp::CLEAR, VK_IMAGE_LAYOUT_GENERAL)
			.end(ColorOp::STORE, VK_IMAGE_LAYOUT_GENERAL)
			.next();

		builder.addDependency()
			.first(VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0)
			.then(0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
			.next();

		builder.addDependency(VK_DEPENDENCY_BY_REGION_BIT)
			.first(0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
			.then(VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_MEMORY_READ_BIT)
			.next();

		builder.addSubpass()
			.addOutput(illum, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.addOutput(illum_prev, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.next();

		pass_denoise = builder.build(device, "Denoise");

	}

	{ // denoise2 2d pass

		RenderPassBuilder builder;

		Attachment::Ref illum = builder.addAttachment(attachment_illumination)
			.begin(ColorOp::CLEAR, VK_IMAGE_LAYOUT_GENERAL)
			.end(ColorOp::STORE, VK_IMAGE_LAYOUT_GENERAL)
			.next();

		builder.addDependency()
			.first(VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0)
			.then(0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
			.next();

		builder.addDependency(VK_DEPENDENCY_BY_REGION_BIT)
			.first(0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
			.then(VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_MEMORY_READ_BIT)
			.next();

		builder.addSubpass()
			.addOutput(illum, VK_IMAGE_LAYOUT_GENERAL)
			.next();

		pass_denoise2 = builder.build(device, "Denoise2");

	}

}

void Renderer::closeRenderPasses() {
	pass_immediate.close();
	pass_compose.close();
	pass_denoise.close();
	pass_denoise2.close();
}

void Renderer::createPipelines() {

	VkExtent2D extent = swapchain.getExtend();

	pipeline_immediate_2d = GraphicsPipelineBuilder::of(device)
		.withViewport(0, 0, extent.width, extent.height)
		.withScissors(0, 0, extent.width, extent.height)
		.withCulling(false)
		.withRenderPass(pass_immediate, 0)
		.withShaders(shader_screen_vertex, shader_atlas_fragment)
		.withBindingLayout(binding_3d)
		.withDescriptorSetLayout(layout_immediate)
		.withBlendMode(BlendMode::ENABLED)
		.withBlendAlphaFunc(VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
		.withBlendColorFunc(VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
		.withPushConstant(mesh_constant)
		.withDepthTest(VK_COMPARE_OP_ALWAYS, true, true)
		.build();

	pipeline_immediate_3d = GraphicsPipelineBuilder::of(device)
		.withViewport(0, 0, extent.width, extent.height)
		.withScissors(0, 0, extent.width, extent.height)
		.withCulling(true, VK_FRONT_FACE_COUNTER_CLOCKWISE)
		.withRenderPass(pass_immediate, 0)
		.withShaders(shader_world_vertex, shader_atlas_fragment)
		.withBindingLayout(binding_3d)
		.withDescriptorSetLayout(layout_immediate)
		.withBlendMode(BlendMode::ENABLED)
		.withBlendAlphaFunc(VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
		.withBlendColorFunc(VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
		.withPushConstant(mesh_constant)
		.withDepthTest(VK_COMPARE_OP_LESS_OR_EQUAL, true, true)
		.build();

	pipeline_text_2d = GraphicsPipelineBuilder::of(device)
		.withViewport(0, 0, extent.width, extent.height)
		.withScissors(0, 0, extent.width, extent.height)
		.withCulling(false)
		.withRenderPass(pass_immediate, 0)
		.withShaders(shader_screen_vertex, shader_text_fragment)
		.withBindingLayout(binding_3d)
		.withDescriptorSetLayout(layout_immediate)
		.withBlendMode(BlendMode::ENABLED)
		.withBlendAlphaFunc(VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
		.withBlendColorFunc(VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
		.withPushConstant(mesh_constant)
		.withDepthTest(VK_COMPARE_OP_ALWAYS, true, true)
		.build();

	pipeline_compose_2d = GraphicsPipelineBuilder::of(device)
		.withViewport(0, 0, extent.width, extent.height)
		.withScissors(0, 0, extent.width, extent.height)
		.withCulling(false)
		.withRenderPass(pass_compose, 0)
		.withShaders(shader_blit_vertex, shader_blit_fragment)
		.withDescriptorSetLayout(layout_compose)
		.withDepthTest(VK_COMPARE_OP_ALWAYS, true, true)
		.build();

	pipeline_denoise_2d = GraphicsPipelineBuilder::of(device)
		.withViewport(0, 0, extent.width, extent.height)
		.withScissors(0, 0, extent.width, extent.height)
		.withCulling(false)
		.withRenderPass(pass_denoise, 0)
		.withShaders(shader_blit_vertex, shader_denoise_fragment)
		.withDescriptorSetLayout(layout_denoise)
		.withDepthTest(VK_COMPARE_OP_ALWAYS, true, false)
		.build();

	pipeline_denoise2_2d = GraphicsPipelineBuilder::of(device)
		.withViewport(0, 0, extent.width, extent.height)
		.withScissors(0, 0, extent.width, extent.height)
		.withCulling(false)
		.withRenderPass(pass_denoise2, 0)
		.withShaders(shader_blit_vertex, shader_denoise2_fragment)
		.withDescriptorSetLayout(layout_denoise2)
		.withDepthTest(VK_COMPARE_OP_ALWAYS, true, false)
		.build();

	ShaderTableBuilder builder;
	builder.addMissShader(shader_trace_miss);
	builder.addMissShader(shader_trace_shadow_miss);
	builder.addRayGenShader(shader_trace_gen);
	builder.addHitGroup().withClosestHit(shader_trace_hit);

	ShaderTableLayout shader_layout = builder.build();

	pipeline_trace_3d = RaytracePipelineBuilder::of(device)
		.withRecursionDepth(2)
		.withDescriptorSetLayout(layout_raytrace)
		.withShaderLayout(shader_layout)
		.build();

	shader_table.close();
	shader_table = shader_layout.allocate(device, allocator, pipeline_trace_3d);

}

void Renderer::closePipelines() {
	pipeline_immediate_2d.close();
	pipeline_immediate_3d.close();
	pipeline_text_2d.close();
	pipeline_trace_3d.close();
	pipeline_compose_2d.close();
	pipeline_denoise_2d.close();
	pipeline_denoise2_2d.close();
}

void Renderer::closeFrames() {
	if (!frames.empty()) {

		// execute all pending operations to not leak any memory
		for (RenderFrame& frame : frames) {
			frame.execute();
		}

		frames.clear();
		descriptor_pool.reset();
		graphics_pool.reset(true);
		transient_pool.reset(true);
	}
}

void Renderer::createFrames() {
	for (int i = 0; i < concurrent; i ++) {
		frames.emplace_back(*this, graphics_pool, device);
	}
}

void Renderer::lateClose() {

	// close all attachments
	attachment_depth.close(device);
	attachment_albedo.close(device);
	attachment_illumination.close(device);
	attachment_prev_illumination.close(device);
	attachment_normal.close(device);
	attachment_prev_normal.close(device);
	attachment_illum_transport.close(device);
	attachment_soild_illumination.close(device);
	attachment_world_position.close(device);
	attachment_prev_world_position.close(device);

	swapchain.close();
	closeFrames();
	closePipelines();
}

void Renderer::lateInit() {
	createSwapchain();
	createPipelines();
	createFrames();

	Fence fence = createFence();
	CommandBuffer buffer = transient_pool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	CommandRecorder recorder = buffer.record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	prepareForRendering(recorder);

	recorder.done();
	buffer.submit().signal(fence).done(queue);
	fence.wait();
	fence.close();
}

void Renderer::prepareForRendering(CommandRecorder& recorder) {
	recorder.transitionLayout(attachment_albedo, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED);
	recorder.transitionLayout(attachment_illumination, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED);
	recorder.transitionLayout(attachment_prev_illumination, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED);
	recorder.transitionLayout(attachment_normal, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED);
	recorder.transitionLayout(attachment_prev_normal, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED);
	recorder.transitionLayout(attachment_illum_transport, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED);
	recorder.transitionLayout(attachment_soild_illumination, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED);
	recorder.transitionLayout(attachment_world_position, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED);
	recorder.transitionLayout(attachment_prev_world_position, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED);
}

RenderFrame& Renderer::getFrame() {
	return frames.data()[index];
}

void Renderer::acquirePresentationIndex() {
	if (swapchain.getNextImage(getFrame().available_semaphore, &this->current_image)) {
		reload();
	}
}

void Renderer::presentFramebuffer() {
	auto semaphore = getFrame().finished_semaphore;

	if (swapchain.present(queue, semaphore, this->current_image)) {
		reload();
		printf("WARN: Reloading during framebuffer presentation!\n");
	}
}

void Renderer::rebuildTopLevel(CommandRecorder& recorder) {
	AccelStructConfig config = AccelStructConfig::create(AccelStructConfig::BUILD, AccelStructConfig::TOP)
		.addInstances(device, instances->count(), instances->getInstanceBuffer(), true)
		.setFlags(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR)
		.setDebugName("TLAS");

	instances->flush(recorder);

	// Recreate TLAS and update descriptors
	tlas.close(device);
	tlas = bakery.submit(device, allocator, config)->getStructure();
	getFrame().set_raytrace.structure(0, tlas);

	bakery.bake(device, allocator, recorder);

	recorder.memoryBarrier()
		.first(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR)
		.then(VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR)
		.done();

}

void Renderer::rebuildBottomLevel(CommandRecorder& recorder) {
	// wait for object transfer
	recorder.memoryBarrier()
		.first(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT)
		.then(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_MEMORY_READ_BIT)
		.done();

	bakery.bake(device, allocator, recorder);
}

ImmediateRenderer& Renderer::getImmediateRenderer() {
	return immediate;
}

Fence Renderer::createFence(bool signaled) {
	return {device.getHandle(), signaled};
}

Semaphore Renderer::createSemaphore() {
	return {device.getHandle()};
}

PushConstant Renderer::createPushConstant(VkShaderStageFlags stages, uint32_t bytes) {
	if (bytes > physical->getLimits().maxPushConstantsSize) {
		throw std::runtime_error {"The reqested push constant is too large!"};
	}

	return {stages, bytes};
}

Renderer::Renderer(ApplicationParameters& parameters)
: windows(), window(windows.open(parameters.width, parameters.height, parameters.getTitle())) {

	index = 0;
	concurrent = 1;
	surface_format = VK_FORMAT_B8G8R8A8_SRGB;
	frames.reserve(concurrent);
	instances = std::make_unique<InstanceManager>();

	// early init
	createInstance(parameters);
	glfwCreateWindowSurface(instance.getHandle(), window->getHandle(), nullptr, &surface);
	pickDevice();

	// family is selected and loaded as part of device picking
	queue = device.getQueue(family);

	// create command buffer pools for our family
	transient_pool = CommandPool::create(device, family, true);
	graphics_pool = CommandPool::create(device, family, false);

	allocator = Allocator {device, *physical, instance};
	allocator.print();

	// create vertex bindings
	binding_3d = BindingLayoutBuilder::begin()
		.attribute(0, Vertex3D::position)
		.attribute(1, Vertex3D::color)
		.attribute(2, Vertex3D::texture)
		.attribute(3, Vertex3D::material)
		.attribute(4, Vertex3D::padding)
		.done();

	// create descriptor layouts
	layout_immediate = DescriptorSetLayoutBuilder::begin()
		.descriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.descriptor(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.done(device);

	layout_compose = DescriptorSetLayoutBuilder::begin()
		.descriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.descriptor(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.descriptor(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.descriptor(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.descriptor(4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.descriptor(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.done(device);

	layout_denoise2 = DescriptorSetLayoutBuilder::begin()
		.descriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.descriptor(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.descriptor(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.done(device);

	layout_denoise = DescriptorSetLayoutBuilder::begin()
		.descriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.descriptor(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.descriptor(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.done(device);

	layout_raytrace = DescriptorSetLayoutBuilder::begin()
		.descriptor(0, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR)
		.descriptor(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR)
		.descriptor(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR)
		.descriptor(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR)
		.descriptor(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, TextureManager::MAX_TEXTURES)
		.descriptor(5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR)
		.descriptor(6, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR)
		.descriptor(7, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR)
		.descriptor(8, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR)
		.descriptor(9, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR)
		.descriptor(10, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR)
		.descriptor(11, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR)
		.descriptor(12, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR)
		.descriptor(13, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR)
		.done(device);

	// add layouts to the pool so that they can be allocated
	descriptor_pool = DescriptorPoolBuilder::begin()
		.addDynamic(layout_immediate, 1)
		.addDynamic(layout_raytrace, 1)
		.addDynamic(layout_compose, 1)
		.addDynamic(layout_denoise2, 1)
		.addDynamic(layout_denoise, 1)
		.done(device, concurrent);

	// render pass used during mesh rendering
	mesh_constant = createPushConstant(VK_SHADER_STAGE_VERTEX_BIT, sizeof(MeshConstant));

	createAttachments();
	createRenderPasses();
	createShaders();

	// begin late initialization
	lateInit();

}

Renderer::~Renderer() {

	wait();

	// late close is actually first :)
	lateClose();

	// close all the descriptor layouts here
	layout_immediate.close();
	layout_raytrace.close();
	layout_compose.close();
	layout_denoise.close();
	layout_denoise2.close();

	descriptor_pool.close();
	transient_pool.close();
	graphics_pool.close();

	// destroy messenger only if it was created before
	if (messenger) {
		Proxy::vkDestroyDebugUtilsMessengerEXT(instance.getHandle(), messenger, nullptr);
	}

	vkDestroySurfaceKHR(instance.getHandle(), surface, nullptr);
	instances.reset();
	immediate.close(device);
	materials.close(device);
	lights.close();

	// It's important to maintain the correct order
	closeRenderPasses();
	bakery.close();
	tlas.close(device);
	shader_table.close();

	shader_world_vertex.close();
	shader_screen_vertex.close();
	shader_atlas_fragment.close();
	shader_text_fragment.close();
	shader_trace_gen.close();
	shader_trace_miss.close();
	shader_trace_shadow_miss.close();
	shader_trace_hit.close();
	shader_blit_vertex.close();
	shader_blit_fragment.close();
	shader_denoise_fragment.close();
	shader_denoise2_fragment.close();

	VulkanDebug::assertAllDead();
	allocator.close();
	device.close();
	instance.close();

	printf("INFO: Renderer deinitialized, goodbye!\n");

}

void Renderer::reload() {
	wait();
	lateClose();
	lateInit();
}

Window& Renderer::getWindow() const {
	return *window;
}

void Renderer::draw() {

	int w, h;
	window->getFramebufferSize(&w, &h);

	if (w == 0 || h == 0) {
		immediate.clear();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 30));
		return;
	}

	RenderFrame& frame = getFrame();

	frame.wait();
	frame.execute();

	acquirePresentationIndex();

	// begin rendering
	recorder = frame.buffer.record();
	frame.flushUniformBuffer(recorder);

	rebuildTopLevel(recorder);

	auto& buffer = instances->getAttachmentBuffer();
	frame.set_raytrace.buffer(3, buffer.getBuffer(), buffer.getBuffer().size());

	auto& material_buffer = materials.getMaterialBuffer();
	frame.set_raytrace.buffer(5, material_buffer.getBuffer(), material_buffer.getBuffer().size());

	lights.flush(recorder);
	auto& light_buffer = lights.getBuffer();
	frame.set_raytrace.buffer(13, light_buffer.getBuffer(), light_buffer.getBuffer().size());

	materials.getTextureManager().updateDescriptorSet(device, frame.set_raytrace, 4);

	// wait for uniform transfer before raytracing or rasterization starts
	recorder.memoryBarrier()
		.first(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT)
		.then(VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_UNIFORM_READ_BIT)
		.done();

	recorder.transitionLayout(attachment_depth, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED);

	// ray trace
	recorder.bindPipeline(pipeline_trace_3d)
		.bindDescriptorSet(frame.set_raytrace)
		.traceRays(shader_table, width(), height());

	if (parameters.getDenoise()) {
		// denoise
		recorder.beginRenderPass(pass_denoise, current_image, swapchain.getExtend())
			.bindPipeline(pipeline_denoise_2d)
			.bindDescriptorSet(frame.set_denoise)
			.draw(3)
			.endRenderPass();

		// second denoise pass
		recorder.beginRenderPass(pass_denoise2, current_image, swapchain.getExtend())
			.bindPipeline(pipeline_denoise2_2d)
			.bindDescriptorSet(frame.set_denoise2)
			.draw(3)
			.endRenderPass();
	}

	// compose final image
	recorder.beginRenderPass(pass_compose, current_image, swapchain.getExtend())
		.bindPipeline(pipeline_compose_2d)
		.bindDescriptorSet(frame.set_compose)
		.draw(3)
		.endRenderPass();

	// upload buffers and textures
	immediate.upload(recorder);
	frame.set_immediate.sampler(0, immediate.getAtlasTexture(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	// draw immediate vertex data
	recorder.beginRenderPass(pass_immediate, current_image, swapchain.getExtend());
	recorder.bindPipeline(pipeline_immediate_3d);
	recorder.bindDescriptorSet(frame.set_immediate);
	immediate.basic_3d.draw(mesh_constant, recorder);

	recorder.bindPipeline(pipeline_immediate_2d);
	immediate.basic.draw(mesh_constant, recorder);

	recorder.bindPipeline(pipeline_text_2d);
	immediate.text.draw(mesh_constant, recorder);

	recorder.endRenderPass();

	recorder.done();

	frame.buffer.submit()
		.awaits(frame.available_semaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
		.signal(frame.finished_semaphore)
		.signal(frame.flight_fence)
		.done(queue);

	presentFramebuffer();

	// next frame
	index = (index + 1) % concurrent;
	immediate.clear();

}

void Renderer::wait() {
	device.wait();
}

int Renderer::width() {
	return swapchain.getExtend().width;
}

int Renderer::height() {
	return swapchain.getExtend().height;
}

MaterialManager& Renderer::getMaterialManager() {
	return materials;
}

LightManager& Renderer::getLightManager() {
	return lights;
}
