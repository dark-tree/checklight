
#include "renderer.hpp"
#include "const.hpp"

#include "render/vulkan/setup/device.hpp"
#include "render/vulkan/buffer/texture.hpp"
#include "render/vulkan/pass/render.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/vulkan/setup/debug.hpp"
#include "render/api/vertex.hpp"
#include "render/api/model.hpp"

/*
 * Renderer
 */

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* user) {
	return ((Renderer*) user)->onMessage(severity, data);
}

VkBool32 Renderer::onMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity, const VkDebugUtilsMessengerCallbackDataEXT* data) {
	
	auto message = std::string(data->pMessage);
	if (message.find("VUID-vkCmdBuildAccelerationStructuresKHR-pInfos-03813") != std::string::npos) {
		return VK_FALSE;
	}

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

	// needed for the vkGetPhysicalDeviceFeatures2KHR which we will need to
	// use any extensions or new vulkan features
	extension.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	#ifdef ENGINE_DEBUG
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
	#ifdef ENGINE_DEBUG
		create_info.pNext = &messenger_info;
	#endif

	VkInstance vk_instance;

	if (vkCreateInstance(&create_info, nullptr, &vk_instance) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create Vulkan instance!"};
	}

	// store for later use
	this->instance = vk_instance;

	// attach debug messenger for all calls after instance creation
	#ifdef ENGINE_DEBUG
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
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
	};

	for (auto& device : devices) {

		// we need the device to be able to render to our window
		if (!device->canUseSurface(surface)) {
			continue;
		}

		// check for support of required extensions
		for (auto name : required_extensions) {
			if (!device->hasExtension(name)) {
				continue;
			}
		}

		auto* features_vk12 = (const VkPhysicalDeviceVulkan12Features*) device->getFeatures(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES);
		auto* features_ray = (const VkPhysicalDeviceRayTracingPipelineFeaturesKHR*) device->getFeatures(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR);
		auto* features_accel = (const VkPhysicalDeviceAccelerationStructureFeaturesKHR*) device->getFeatures(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR);

		if (!features_vk12->bufferDeviceAddress) {
			continue;
		}

		if (!features_ray->rayTracingPipeline) {
			continue;
		}

		if (!features_accel->accelerationStructure) {
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
			createDevice(std::move(device), queue_family, required_extensions);
			return;
		}

	}

	throw std::runtime_error {"No device could have been selected!"};
}

void Renderer::createDevice(std::shared_ptr<PhysicalDevice> physical, Family queue_family, const std::vector<const char*>& extensions) {
	printf("INFO: Selected '%s' (queue #%d)\n", physical->getName(), queue_family.getIndex());

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

	// Basic device features
	VkPhysicalDeviceFeatures2KHR features {};
	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features.pNext = &features_vk12;

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

	#ifdef ENGINE_DEBUG
		Proxy::loadDebugDeviceFunctions(this->device);
	#endif

	Proxy::loadDeviceFunctions(this->device);
	Proxy::loadRaytraceFunctions(this->device);

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

	// allocate all attachments (except for color)
	attachment_depth.allocate(device, extent.width, extent.height, allocator);

	// create framebuffers
	pass_basic_3d.prepareFramebuffers(swapchain);

	printf("INFO: Swapchain ready\n");

}

void Renderer::createShaders() {

	shader_basic_vertex = compiler.compileFile(device, "assets/shader/basic.vert", Kind::VERTEX);
	shader_basic_fragment = compiler.compileFile(device, "assets/shader/basic.frag", Kind::FRAGMENT);
	shader_trace_gen = compiler.compileFile(device, "assets/shader/ray-gen.glsl", Kind::RAYGEN);
	shader_trace_miss = compiler.compileFile(device, "assets/shader/ray-miss.glsl", Kind::MISS);
	shader_trace_hit = compiler.compileFile(device, "assets/shader/ray-hit.glsl", Kind::CLOSEST);

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

	// very important UwU
	attachment_color.markSwapchainBacked();

}

void Renderer::createRenderPasses() {

	/*
	 * If you are adding a new RenderPass remember to invoke
	 * prepareFramebuffers() on it at the end of createSwapchain()!
	 */

	{ // basic 3d pass

		RenderPassBuilder builder;

		Attachment::Ref color = builder.addAttachment(attachment_color)
			.begin(ColorOp::CLEAR, VK_IMAGE_LAYOUT_UNDEFINED)
			.end(ColorOp::STORE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
			.next();

		Attachment::Ref depth = builder.addAttachment(attachment_depth)
			.begin(ColorOp::CLEAR, VK_IMAGE_LAYOUT_UNDEFINED)
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

		pass_basic_3d = builder.build(device, "Basic 3D");

	}


}

void Renderer::closeRenderPasses() {
	pass_basic_3d.close();
}

void Renderer::createPipelines() {

	VkExtent2D extent = swapchain.getExtend();

	pipeline_basic_3d = GraphicsPipelineBuilder::of(device)
		.withViewport(0, 0, extent.width, extent.height)
		.withScissors(0, 0, extent.width, extent.height)
		.withCulling(false)
		.withRenderPass(pass_basic_3d, 0)
		.withShaders(shader_basic_vertex, shader_basic_fragment)
		.withBindingLayout(binding_3d)
		.withPushConstant(mesh_constant)
		.withDescriptorSetLayout(layout_geometry)
		.withDepthTest(VK_COMPARE_OP_LESS_OR_EQUAL, true, true)
		.build();

	ShaderArrayBuilder builder;
	builder.addMissShader(shader_trace_miss);
	builder.addRayGenShader(shader_trace_gen);
	builder.addHitGroup().withClosestHit(shader_trace_hit);

	pipeline_trace_3d = RaytracePipelineBuilder::of(device)
		.withRecursionDepth(2)
		.withDescriptorSetLayout(layout_geometry)
		.withShaderLayout(builder.build())
		.build();

}

void Renderer::closePipelines() {
	pipeline_basic_3d.close();
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

	swapchain.close();
	closeFrames();
	closePipelines();
}

void Renderer::lateInit() {
	createSwapchain();
	createPipelines();
	createFrames();
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
	}
}

void Renderer::rebuildTopLevel(CommandRecorder& recorder) {
	AccelStructConfig config = AccelStructConfig::create(AccelStructConfig::BUILD, AccelStructConfig::TOP)
		.addInstances(device, instances->getBuffer(), true)
		.setFlags(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR);

	instances->flush(recorder);

	// wait for object transfer
	recorder.memoryBarrier()
		.first(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_MEMORY_WRITE_BIT)
		.then(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_MEMORY_READ_BIT)
		.done();

	bakery.bake(device, allocator, recorder);

	tlas.close(device);
	tlas = bakery.submit(device, allocator, config)->structure;
	bakery.bake(device, allocator, recorder);

	recorder.memoryBarrier()
		.first(VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR)
		.then(VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR)
		.done();

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
		.done();

	// create descriptor layouts
	layout_geometry = DescriptorSetLayoutBuilder::begin()
		.descriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.done(device);

	// add layouts to the pool so that they can be allocated
	descriptor_pool = DescriptorPoolBuilder::begin()
		.addDynamic(layout_geometry, 1)
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
	layout_geometry.close();

	descriptor_pool.close();
	transient_pool.close();
	graphics_pool.close();

	// destroy messenger only if it was created before
	if (messenger) {
		Proxy::vkDestroyDebugUtilsMessengerEXT(instance.getHandle(), messenger, nullptr);
	}

	vkDestroySurfaceKHR(instance.getHandle(), surface, nullptr);
	instances.reset();

	// It's important to maintain the correct order
	closeRenderPasses();
	compiler.close();
	bakery.close();
	tlas.close(device);

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

void Renderer::beginDraw() {
	RenderFrame& frame = getFrame();

	frame.wait();
	frame.execute();

	acquirePresentationIndex();

	// begin rendering
	recorder = frame.buffer.record();

	rebuildTopLevel(recorder);

	recorder.beginRenderPass(pass_basic_3d, current_image, swapchain.getExtend());
	recorder.bindPipeline(pipeline_basic_3d);
	recorder.bindDescriptorSet(frame.set_0);
}

void Renderer::endDraw() {

	recorder.traceRays();

	recorder.endRenderPass();
	recorder.done();

	RenderFrame& frame = getFrame();

	frame.buffer.submit()
		.awaits(frame.available_semaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
		.signal(frame.finished_semaphore)
		.signal(frame.flight_fence)
		.done(queue);

	presentFramebuffer();

	// next frame
	index = (index + 1) % concurrent;

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