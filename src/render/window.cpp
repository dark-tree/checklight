
#include "window.hpp"

/*
 * WindowSystem
 */

void WindowSystem::glfwErrorCallback(int code, const char* description) {
	printf("[GLFW] Error %d: %s\n", code, description);
}

WindowSystem::WindowSystem() {
	if (!glfwInit()) {
		throw std::runtime_error {"Failed to initialize GLFW!"};
	}

	if (!glfwVulkanSupported()) {
		throw std::runtime_error {"Failed to find vulkan loader!"};
	}

	glfwSetErrorCallback(glfwErrorCallback);
}

WindowSystem::~WindowSystem() {
	glfwTerminate();
	printf("INFO: GLFW terminated\n");
}

std::vector<const char*> WindowSystem::getRequiredExtensions() const {
	uint32_t count = 0;
	const char** array = glfwGetRequiredInstanceExtensions(&count);
	return {array, array + count};
}

std::unique_ptr<Window> WindowSystem::open(uint32_t w, uint32_t h, const std::string& title) const {
	return std::unique_ptr<Window>(new Window {w, h, title});
}

/*
 * Window
 */

void Window::glfwKeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
	Window* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		// TODO: dispatch event
	}
}

void Window::glfwButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods) {
	Window* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		// TODO: dispatch event
	}
}

void Window::glfwScrollCallback(GLFWwindow* glfw_window, double x_scroll, double y_scroll) {
	Window* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		// TODO: dispatch event
	}
}

Window::Window(uint32_t w, uint32_t h, std::string title_string) {

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	#if !defined(NDEBUG)
	title_string += " (Debug Build)";
	#endif

	handle = glfwCreateWindow(w, h, title_string.c_str(), nullptr, nullptr);
	if (handle == nullptr) {
		throw std::runtime_error {"Failed to create GLFW window!"};
	}

	glfwSetWindowUserPointer(handle, this);
	glfwSetInputMode(handle, GLFW_STICKY_KEYS, GL_TRUE);

	// send events to the root input consumer
	glfwSetKeyCallback(handle, glfwKeyCallback);
	glfwSetMouseButtonCallback(handle, glfwButtonCallback);
	glfwSetScrollCallback(handle, glfwScrollCallback);
}

Window::~Window() {
	glfwDestroyWindow(handle);
}

GLFWwindow* Window::getHandle() const {
	return handle;
}

void Window::poll() const {
	glfwPollEvents();
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(handle);
}

void Window::getFramebufferSize(int* width, int* height) const {
	glfwGetFramebufferSize(handle, width, height);
}

bool Window::isKeyPressed(int key) const {
	return glfwGetKey(handle, key) == GLFW_PRESS;
}

bool Window::isButtonPressed(int button) const {
	return glfwGetMouseButton(handle, button) == GLFW_PRESS;
}

void Window::getCursor(double* x, double* y) const {
	glfwGetCursorPos(handle, x, y);
}

void Window::setMouseCapture(bool capture) {
	glfwSetInputMode(handle, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}