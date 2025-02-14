#pragma once

#include "external.hpp"
#include "frame.hpp"
#include "application.hpp"
#include "window.hpp"
#include "render/vulkan/setup/proxy.hpp"
#include "render/vulkan/setup/instance.hpp"
#include "render/vulkan/shader/compiler.hpp"
#include "render/vulkan/command/buffer.hpp"
#include "render/vulkan/setup/swapchain.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/vulkan/setup/allocator.hpp"
#include "render/vulkan/buffer/attachment.hpp"
#include "render/vulkan/descriptor/vertex.hpp"
#include "render/vulkan/pass/render.hpp"
#include "render/vulkan/pass/pipeline.hpp"
#include "render/vulkan/descriptor/pool.hpp"
#include "render/vulkan/descriptor/push.hpp"
#include "render/vulkan/raytrace/instance.hpp"
#include "render/vulkan/raytrace/factory.hpp"

class Renderer {

	private:

		/// This object is quite large so to limit the size of this already huge class
		/// we put it behind a pointer, that class requires no close() call anyway
		std::shared_ptr<PhysicalDevice> physical;

		/// the number of concurrent frames, this value should no be larger then 4-5 to no cause input delay
		/// setting it to 1 effectively disables concurrent frames
		int concurrent;

		/// the index of the next frame to render, this value is kept within
		/// the size of the frames vector (the vector is used as a ring buffer)
		int index;

		/// a ring-buffer line holder for the per frame states, utilized for concurrent
		/// frame rendering (the CPU can "render ahead" of the GPU)
		std::vector<RenderFrame> frames;

	protected:

		friend class RenderFrame;
		friend class RenderMesh;
		friend class RenderCommander;
		friend class ReusableBuffer;
		friend class AccelStructFactory;

		/// The last image index acquired from the driver,
		/// this is used as an offset into a framebuffer set
		uint32_t current_image;

		Compiler compiler;
		WindowSystem windows;
		std::unique_ptr<Window> window;

		// early vulkan objects
		VkFormat surface_format;
		VkDebugUtilsMessengerEXT messenger;
		Instance instance;
		VkSurfaceKHR surface;
		LogicalDevice device;
		Family family;
		Queue queue;
		CommandPool transient_pool;
		CommandPool graphics_pool;
		Allocator allocator;
		DescriptorPool descriptor_pool;

		// raytracing
		std::unique_ptr<InstanceManager> instances;
		AccelStructFactory bakery;
		AccelStruct tlas;
		ShaderTable shader_table;

		// shaders
		Shader shader_basic_vertex;
		Shader shader_basic_fragment;
		Shader shader_trace_gen;
		Shader shader_trace_miss;
		Shader shader_trace_hit;
		Shader shader_blit_vertex;
		Shader shader_blit_fragment;

		// attachments
		Attachment attachment_color;
		Attachment attachment_depth;
		Attachment attachment_albedo;

		// descriptors
		DescriptorSetLayout layout_geometry;
		DescriptorSetLayout layout_compose;
		DescriptorSetLayout layout_raytrace;

		// layouts
		BindingLayout binding_3d;

		// renderpasses
		RenderPass pass_basic_3d;
		RenderPass pass_compose_2d;

		// Pipelines
		GraphicsPipeline pipeline_basic_3d;
		GraphicsPipeline pipeline_trace_3d;
		GraphicsPipeline pipeline_compose_2d;

		// late vulkan objects
		Swapchain swapchain;

		// primary rendering recorder
		CommandRecorder recorder;

		// push constants
		PushConstant mesh_constant;

	private:

		friend VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
		VkBool32 onMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity, const VkDebugUtilsMessengerCallbackDataEXT* data);

		/// Loads Swapchain
		void createSwapchain();

		/// Loads the VkDebugUtilsMessengerEXT, and Instance
		void createInstance(ApplicationParameters& parameters);

		/// Selects a compatible driver and passes control to createDevice
		void pickDevice();

		/// Loads the LogicalDevice, and Family
		void createDevice(std::shared_ptr<PhysicalDevice> device, Family queue_family, const std::vector<const char*>& extensions);

		void createShaders();
		void createAttachments();

		void createRenderPasses();
		void closeRenderPasses();

		void createPipelines();
		void closePipelines();

		void createFrames();
		void closeFrames();

		void lateClose();
		void lateInit();

		void acquirePresentationIndex();
		void presentFramebuffer();
		void rebuildTopLevel(CommandRecorder& recorder);

	protected:


		Fence createFence(bool signaled = false);
		Semaphore createSemaphore();
		PushConstant createPushConstant(VkShaderStageFlags stages, uint32_t bytes);

	public:

		Renderer() = default;
		Renderer(ApplicationParameters& parameters);
		virtual ~Renderer();

		/// Get the currently selected frame
		RenderFrame& getFrame();

		/// Recreate swapchain, this operation is extremely slow
		void reload();

		/// Get the Window to which this renderer is attached
		Window& getWindow() const;

		/// Begins the next frame, all rendering should happen after this call
		void beginDraw();

		/// End the frame, all rendering should happen before this call
		void endDraw();

		/// Synchronize all operations and wait for the GPU to idle
		void wait();

		/// Get current screen width, in pixels
		int width();

		/// Get current screen height, in pixels
		int height();

};
