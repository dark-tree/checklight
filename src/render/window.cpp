
#include "window.hpp"

#include <shared/logger.hpp>

#include "input/input.hpp"

/*
 * Viewport
 */

Viewport viewport {0, 0};

void Viewport::setCurrent(int width, int height) {
	viewport.width = width;
	viewport.height = height;
}

Viewport Viewport::getCurrent() {
	return viewport;
}

/*
 * WindowSystem
 */

WindowSystem::WindowSystem() {
	if (!glfwInit()) {
		FAULT("Failed to initialize GLFW!");
	}

	if (!glfwVulkanSupported()) {
		FAULT("Failed to find Vulkan loader!");
	}

	glfwSetErrorCallback(Window::glfwErrorCallback);
}

WindowSystem::~WindowSystem() {
	glfwTerminate();
	out::info("GLFW terminated");
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
 * InputContext
 */

InputContext::InputContext(const Window& window) {
	this->handle = window.getHandle();
}

bool InputContext::isKeyPressed(int key) const {
	return glfwGetKey(handle, key) == GLFW_PRESS;
}

bool InputContext::isButtonPressed(int button) const {
	return glfwGetMouseButton(handle, button) == GLFW_PRESS;
}

glm::vec2 InputContext::getCursorPosition() const {
	double x, y;
	glfwGetCursorPos(handle, &x, &y);

	return {x, y};
}

/*
 * Window
 */

void Window::glfwKeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		glm::vec2 mouse = window->getInputContext().getCursorPosition();
		window->getInputDispatcher().onEvent(KeyboardEvent {key, scancode, action, mods, mouse.x, mouse.y});
	}
}

void Window::glfwButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods) {
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		glm::vec2 mouse = window->getInputContext().getCursorPosition();
		window->getInputDispatcher().onEvent(ButtonEvent {button, action, mods, mouse.x, mouse.y});
	}
}

void Window::glfwScrollCallback(GLFWwindow* glfw_window, double horizontal, double vertical) {
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		glm::vec2 mouse = window->getInputContext().getCursorPosition();
		window->getInputDispatcher().onEvent(ScrollEvent {horizontal, vertical, mouse.x, mouse.y});
	}
}

void Window::glfwCursorCallback(GLFWwindow* glfw_window, double x, double y) {
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		MouseEvent event {x, y};
		window->getInputDispatcher().onEvent(event);
	}
}

void Window::glfwUnicodeCallback(GLFWwindow* glfw_window, unsigned int unicode) {
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		glm::vec2 mouse = window->getInputContext().getCursorPosition();
		window->getInputDispatcher().onEvent(UnicodeEvent {unicode, mouse.x, mouse.y});
	}
}

void Window::glfwWindowCloseCallback(GLFWwindow* glfw_window) {
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	if (window) {
		CloseEvent event {};
		window->getInputDispatcher().onEvent(event);

		glfwSetWindowShouldClose(glfw_window, !event.abort_flag);
	}
}

void Window::glfwWindowResizeCallback(GLFWwindow* glfw_window, int width, int height) {
	auto* window = (Window*) glfwGetWindowUserPointer(glfw_window);

	Viewport::setCurrent(width, height);

	if (window) {
		window->getInputDispatcher().onEvent(ResizeEvent {width, height});
	}
}

void Window::glfwErrorCallback(int error_code, const char* description) {
	out::error("GLFW [%d]: %s", error_code, description);
}

Window::Window(uint32_t w, uint32_t h, std::string title_string) {

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	Viewport::setCurrent(w, h);

	#if !defined(NDEBUG)
	title_string += " (Debug Build)";
	#endif

	handle = glfwCreateWindow(w, h, title_string.c_str(), nullptr, nullptr);
	if (handle == nullptr) {
		FAULT("Failed to create GLFW window!");
	}

	glfwSetWindowUserPointer(handle, this);

	// send events to the root input consumer
	glfwSetKeyCallback(handle, glfwKeyCallback);
	glfwSetMouseButtonCallback(handle, glfwButtonCallback);
	glfwSetScrollCallback(handle, glfwScrollCallback);
	glfwSetCursorPosCallback(handle, glfwCursorCallback);
	glfwSetCharCallback(handle, glfwUnicodeCallback);
	glfwSetWindowCloseCallback(handle, glfwWindowCloseCallback);
	glfwSetWindowSizeCallback(handle, glfwWindowResizeCallback);
}

Window::~Window() {
	glfwDestroyWindow(handle);

	for (auto [id, cursor] : cursors) {
		glfwDestroyCursor(cursor);
	}

	out::info("Unloaded %d cursors", (int) cursors.size());
	cursors.clear();
}

GLFWwindow* Window::getHandle() const {
	return handle;
}

void Window::poll() {
	glfwPollEvents();

	FrameEvent event {};
	dispatcher.onEvent(event);

	setMouseCapture(event.capture_flag);
	setShouldClose(event.close_flag);
	setMouseIcon(event.icon);
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

InputContext Window::getInputContext() {
	return {*this};
}

void Window::setMouseCapture(bool capture) {
	if (capture != this->capture) {
		this->capture = capture;

		glfwSetInputMode(handle, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
}

void Window::setShouldClose(bool close) {
	if (close != this->close) {
		this->close = close;

		glfwSetWindowShouldClose(handle, close);
	}
}

void Window::setMouseIcon(CursorIcon::Icon cursor) {
	if (icon == cursor) {
		return;
	}

	auto it = cursors.find(cursor);
	GLFWcursor* pointer = nullptr;

	if (it != cursors.end()) {
		pointer = it->second;
	} else {
		out::debug("Loaded standard cursor icon '%s'", CursorIcon::toString(cursor));
		pointer = glfwCreateStandardCursor(cursor);
		cursors[cursor] = pointer;
	}

	icon = cursor;
	glfwSetCursor(handle, pointer);
}