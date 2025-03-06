#pragma once

#include <input/event/frame.hpp>

#include "external.hpp"
#include "input/dispatcher.hpp"

class Window;

class WindowSystem {

	private:

		static void glfwErrorCallback(int code, const char* description);

	public:

		WindowSystem();
		~WindowSystem();

		/// The list of extensions required from vulkan by the window system
		std::vector<const char*> getRequiredExtensions() const;

		/// Open new window of the given size
		std::unique_ptr<Window> open(uint32_t w, uint32_t h, const std::string& title) const;

};

class InputContext {

	private:

		friend class Window;
		GLFWwindow* handle;

		InputContext(const Window& window);

	public:

		bool isKeyPressed(int key) const;
		bool isButtonPressed(int button) const;
		glm::vec2 getCursorPosition() const;


};

class Window {

	private:

		bool capture = false;
		bool close = false;
		CursorIcon::Icon icon = CursorIcon::DEFAULT;

		std::unordered_map<CursorIcon::Icon, GLFWcursor*> cursors;
		InputDispatcher dispatcher;
		GLFWwindow* handle;

		static void glfwKeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
		static void glfwButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods);
		static void glfwScrollCallback(GLFWwindow* glfw_window, double x, double y);
		static void glfwCursorCallback(GLFWwindow* glfw_window, double x, double y);
		static void glfwUnicodeCallback(GLFWwindow* glfw_window, unsigned int unicode);
		static void glfwWindowCloseCallback(GLFWwindow* glfw_window);
		static void glfwWindowResizeCallback(GLFWwindow* glfw_window, int width, int height);
		static void glfwErrorCallback(int error_code, const char* description);

		friend class WindowSystem;
		Window(uint32_t w, uint32_t h, std::string title);

		// Controlled using FrameEvent
		void setMouseCapture(bool capture);
		void setShouldClose(bool close);
		void setMouseIcon(CursorIcon::Icon cursor);

	public:

		~Window();

		GLFWwindow* getHandle() const;

		/// Ask the OS for input events and invoke handlers
		void poll();

		/// Check if the user wants to close this window
		bool shouldClose() const;

		/// Get the current size of the window's "paintable" area
		void getFramebufferSize(int* width, int* height) const;

		InputDispatcher& getInputDispatcher();
		InputContext getInputContext();

};
