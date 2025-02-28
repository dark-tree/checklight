#pragma once

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


class Window {

	private:

		InputDispatcher dispatcher;
		GLFWwindow* handle;

		static void glfwKeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
		static void glfwButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods);
		static void glfwScrollCallback(GLFWwindow* glfw_window, double x, double y);
		static void glfwCursorCallback(GLFWwindow* glfw_window, double x, double y);
		static void glfwUnicodeCallback(GLFWwindow* glfw_window, unsigned int unicode);
		static void glfwErrorCallback(int error_code, const char* description);

		friend class WindowSystem;
		Window(uint32_t w, uint32_t h, std::string title);

	public:

		~Window();

		GLFWwindow* getHandle() const;

		/// Ask the OS for input events and invoke handlers
		void poll() const;

		/// Check if the user wants to close this window
		bool shouldClose() const;

		/// Get the current size of the window's "paintable" area
		void getFramebufferSize(int* width, int* height) const;

		InputDispatcher& getInputDispatcher();

		/// Enable or disable mouse capture, a captured mouse can't leave the window and is invisible
		void setMouseCapture(bool capture);

		/// Check if a key is pressed, for debug use only!
		bool isKeyPressed(int key) const;

		/// Check if a mouse button is pressed, for debug use only!
		bool isButtonPressed(int button) const;		

		/// Get mouse cursor position, for debug use only!
		glm::vec2 getCursor() const;

};
