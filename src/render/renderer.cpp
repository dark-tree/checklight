
#include "renderer.hpp"
#include "const.hpp"

/*
 * Renderer
 */

void Renderer::createInstance() {

	// optional info about our application
//	VkApplicationInfo app_info {};
//	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//	app_info.pApplicationName = name;
//	app_info.applicationVersion = VK_MAKE_VERSION(major, minor, patch);
//	app_info.pEngineName = ENGINE_NAME;
//	app_info.engineVersion = ENGINE_VERSION;
//	app_info.apiVersion = VK_API_VERSION_1_0;
//
//	// information required for creating an instance
//	VkInstanceCreateInfo create_info {};
//	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//	create_info.pApplicationInfo = &app_info;
//	create_info.enabledExtensionCount = instance_extensions.size();
//	create_info.ppEnabledExtensionNames = instance_extensions.data();
//	create_info.enabledLayerCount = validation_layers.size();
//	create_info.ppEnabledLayerNames = validation_layers.data();

}

Renderer::Renderer(ApplicationParameters& parameters)
: windows(), window(windows.open(parameters.width, parameters.height, parameters.getTitle())) {



}

Renderer::~Renderer() {

}

Window& Renderer::getWindow() const {
	return *window;
}