#pragma once

#include "external.hpp"
#include "application.hpp"
#include "window.hpp"
#include "vulkan/proxy.hpp"
#include "vulkan/instance.hpp"
#include "vulkan/compiler.hpp"
#include "vulkan/command.hpp"

class Renderer {

	private:

		Compiler compiler;
		WindowSystem windows;
		std::unique_ptr<Window> window;

		// early vulkan objects
		VkDebugUtilsMessengerEXT messenger;
		Instance instance;
		VkSurfaceKHR surface;
		PhysicalDevice physical;
		LogicalDevice device;
		Family family;
		Queue queue;
		CommandPool transient_pool;
		CommandPool graphics_pool;

		// late vulkan objects
		Swapchain swapchain;

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

		void createAttachments();
		void createRenderPasses();
		void createFramebuffers();
		void createPipelines();
		void createFrames();

		/// Close resources created during lateInit()
		void lateClose();

		/// Perform late initialization, this step needs to re-run each time the swapchain requires to be rebuild
		void lateInit();

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

};
