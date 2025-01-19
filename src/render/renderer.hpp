#pragma once

#include "external.hpp"
#include "frame.hpp"
#include "application.hpp"
#include "window.hpp"
#include "vulkan/proxy.hpp"
#include "vulkan/instance.hpp"
#include "vulkan/compiler.hpp"
#include "vulkan/command.hpp"
#include "vulkan/swapchain.hpp"
#include "vulkan/buffer.hpp"
#include "render/vulkan/allocator.hpp"
#include "render/vulkan/attachment.hpp"
#include "render/vulkan/binding.hpp"
#include "render/vulkan/pass/render.hpp"
#include "render/vulkan/pass/pipeline.hpp"
#include "render/vulkan/descriptor/pool.hpp"

class Renderer {

	private:

		/// the number of concurrent frames, this value should no be larger then 4-5 to no cause input delay
		/// setting it to 1 effectively disables concurrent frames
		int concurrent; // todo

		/// the index of the next frame to render, this value is kept within
		/// the size of the frames vector (the vector is used as a ring buffer)
		int index;

		/// a ring-buffer line holder for the per frame states, utilized for concurrent
		/// frame rendering (the CPU can "render ahead" of the GPU)
		std::vector<RenderFrame> frames;

	private:

		friend class RenderFrame;

		Compiler compiler;
		WindowSystem windows;
		std::unique_ptr<Window> window;

		// early vulkan objects
		VkFormat surface_format;
		VkDebugUtilsMessengerEXT messenger;
		Instance instance;
		VkSurfaceKHR surface;
		PhysicalDevice physical;
		LogicalDevice device;
		Family family;
		Queue queue;
		CommandPool transient_pool;
		CommandPool graphics_pool;
		Allocator allocator;
		DescriptorPool descriptor_pool;

		// shaders
		Shader shader_basic_vertex;
		Shader shader_basic_fragment;

		// attachments
		Attachment attachment_color;

		// descriptors
		DescriptorSetLayout layout_geometry;

		// layouts
		BindingLayout binding_3d;

		// renderpasses
		RenderPass pass_basic_3d;

		// Pipelines
		GraphicsPipeline pipeline_basic_3d;

		// late vulkan objects
		Swapchain swapchain;

		Buffer vertex_buffer;

	private:

		friend VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
		VkBool32 onMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity, const VkDebugUtilsMessengerCallbackDataEXT* data);

		/// Loads Swapchain
		void createSwapchain();

		/// Loads the VkDebugUtilsMessengerEXT, and Instance
		void createInstance(ApplicationParameters& parameters, bool debug);

		/// Selects a compatible driver and passes control to createDevice
		void pickDevice();

		/// Loads the LogicalDevice, and Family
		void createDevice(PhysicalDevice device, Family queue_family);

		void createShaders();
		void createAttachments();
		void createRenderPasses();
		void createPipelines();
		void createFrames();

		void closeFrames();

		/// Close resources created during lateInit()
		void lateClose();

		/// Perform late initialization, this step needs to re-run each time the swapchain requires to be rebuild
		void lateInit();

		RenderFrame& getFrame();

		uint32_t acquirePresentationIndex();
		void presentFramebuffer(uint32_t index);

	public:

		Renderer(ApplicationParameters& parameters);
		~Renderer();

		/**
		 * @brief Recreate swapchain, this operation is extremely slow
		 */
		void reload();

		/**
		 * @brief Get the Window to which this renderer is attached
		 */
		Window& getWindow() const;

		/**
		 * Render the frame
		 */
		void draw();

};
