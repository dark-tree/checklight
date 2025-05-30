#pragma once

#include "external.hpp"
#include "frame.hpp"
#include "application.hpp"
#include "window.hpp"
#include "immediate.hpp"
#include "parameters.hpp"

#include "render/vulkan/setup/proxy.hpp"
#include "render/vulkan/setup/instance.hpp"
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
#include "render/asset/material.hpp"
#include "render/asset/asset.hpp"
#include "render/asset/light.hpp"

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

		/// a ring-buffer like holder for the per frame states, utilized for concurrent
		/// frame rendering (the CPU can "render ahead" of the GPU)
		std::vector<RenderFrame> frames;

	protected:

		friend class RenderFrame;
		friend class RenderMesh;
		friend class RenderModel;
		friend class RenderCommander;
		friend class ReusableBuffer;
		friend class AccelStructFactory;
		friend class DynamicAtlas;

		/// The last image index acquired from the driver,
		/// this is used as an offset into a framebuffer set
		uint32_t current_image;

		AssetLoader assets;
		WindowSystem windows;
		std::unique_ptr<Window> window;
		ImmediateRenderer immediate;
		MaterialManager materials;
		LightManager lights;
		RenderParameters parameters;

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
		std::shared_ptr<RenderModel> tlas;
		ShaderTable shader_table;

		// shaders
		Shader shader_world_vertex;
		Shader shader_screen_vertex;
		Shader shader_atlas_fragment;
		Shader shader_text_fragment;
		Shader shader_trace_gen;
		Shader shader_trace_miss;
		Shader shader_trace_shadow_miss;
		Shader shader_trace_hit;
		Shader shader_blit_vertex;
		Shader shader_blur_fragment;
		Shader shader_denoise_fragment;
		Shader shader_denoise2_fragment;

		// attachments
		Attachment attachment_screen;
		Attachment attachment_color_msaa;
		Attachment attachment_depth_msaa;
		Attachment attachment_albedo;
		Attachment attachment_illumination;
		Attachment attachment_prev_illumination;
		Attachment attachment_normal;
		Attachment attachment_prev_normal;
		Attachment attachment_illum_transport;
		Attachment attachment_soild_illumination;
		Attachment attachment_world_position;
		Attachment attachment_prev_world_position;

		// descriptors
		DescriptorSetLayout layout_immediate;
		DescriptorSetLayout layout_compose;
		DescriptorSetLayout layout_raytrace;
		DescriptorSetLayout layout_denoise;
		DescriptorSetLayout layout_denoise2;

		// layouts
		BindingLayout binding_3d;

		// renderpasses
		RenderPass pass_immediate;
		RenderPass pass_compose;
		RenderPass pass_denoise;
		RenderPass pass_denoise2;

		// Pipelines
		GraphicsPipeline pipeline_immediate_2d;
		GraphicsPipeline pipeline_immediate_3d;
		GraphicsPipeline pipeline_text_2d;
		GraphicsPipeline pipeline_trace_3d;
		GraphicsPipeline pipeline_compose_2d;
		GraphicsPipeline pipeline_denoise_2d;
		GraphicsPipeline pipeline_denoise2_2d;

		// late vulkan objects
		Swapchain swapchain;

		// primary rendering recorder
		CommandRecorder recorder;

		// push constants
		PushConstant mesh_constant;

		// current multisampling anti-aliasing setting
		VkSampleCountFlagBits msaa;

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
		void createDevice(std::shared_ptr<PhysicalDevice> device, Family queue_family, std::vector<const char*>& extensions, bool multisampling);

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
		void prepareForRendering(CommandRecorder& recorder);

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

		/// Render the next frame, all rendering should happen inside this call
		virtual void draw();

		/// Synchronize all operations and wait for the GPU to idle
		void wait();

		/// Get current screen width, in pixels
		int width();

		/// Get current screen height, in pixels
		int height();

		/// Build pending acceleration structures
		void rebuildBottomLevel(CommandRecorder& recorder);

		/// Get the material manager
		MaterialManager& getMaterialManager();

		/// Get the light manager
		LightManager& getLightManager();

};
