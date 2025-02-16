
#include "window.hpp"
#include "input/input.hpp"

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
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		glm::vec2 mouse = window->getCursor();
		window->getInputDispatcher().onEvent(KeyboardEvent {key, scancode, action, mods, mouse.x, mouse.y});
	}
}

void Window::glfwButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods) {
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		glm::vec2 mouse = window->getCursor();
		window->getInputDispatcher().onEvent(ButtonEvent {button, action, mods, mouse.x, mouse.y});
	}
}

void Window::glfwScrollCallback(GLFWwindow* glfw_window, double horizontal, double vertical) {
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		glm::vec2 mouse = window->getCursor();
		window->getInputDispatcher().onEvent(ScrollEvent {horizontal, vertical, mouse.x, mouse.y});
	}
}

void Window::glfwCursorCallback(GLFWwindow* glfw_window, double x, double y) {
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		MouseEvent event {x, y};
		window->getInputDispatcher().onEvent(event);

		// update mouse capture state
		window->setMouseCapture(event.capture);
	}
}

void Window::glfwUnicodeCallback(GLFWwindow* glfw_window, unsigned int unicode) {
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		glm::vec2 mouse = window->getCursor();
		window->getInputDispatcher().onEvent(UnicodeEvent {unicode, mouse.x, mouse.y});
	}
}

void Window::glfwErrorCallback(int error_code, const char* description) {
	printf("ERROR: GLFW [%d]: %s\n", error_code, description);
}

Window::Window(uint32_t w, uint32_t h, std::string title_string) {

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwSetErrorCallback(glfwErrorCallback);

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
	glfwSetCursorPosCallback(handle, glfwCursorCallback);
	glfwSetCharCallback(handle, glfwUnicodeCallback);
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

InputDispatcher& Window::getInputDispatcher() {
	return dispatcher;
}

bool Window::isKeyPressed(int key) const {
	return glfwGetKey(handle, key) == GLFW_PRESS;
}

bool Window::isButtonPressed(int button) const {
	return glfwGetMouseButton(handle, button) == GLFW_PRESS;
}

glm::vec2 Window::getCursor() const {
	double x, y;
	glfwGetCursorPos(handle, &x, &y);

	return {x, y};
}

void Window::setMouseCapture(bool capture) {
	if (capture != this->capture) {
		this->capture = capture;

		glfwSetInputMode(handle, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
}