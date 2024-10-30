#pragma once

#include "external.hpp"
#include "application.hpp"
#include "window.hpp"
#include "render/vulkan/proxy.hpp"
#include "render/vulkan/instance.hpp"

class Renderer {

	private:

		WindowSystem windows;
		std::unique_ptr<Window> window;

		// early vulkan objects
		VkDebugUtilsMessengerEXT messenger;
		VkSurfaceKHR surface;
		Instance instance;

	private:

		friend VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
		VkBool32 onMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity, const VkDebugUtilsMessengerCallbackDataEXT* data);

		// early init
		void createInstance(ApplicationParameters& parameters, bool debug);
		void pickDevice();
		void createDevice(PhysicalDevice device, Family family);

	public:

		Renderer(ApplicationParameters& parameters);
		~Renderer();

		Window& getWindow() const;

};
