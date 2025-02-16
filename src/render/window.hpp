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

		std::vector<const char*> getRequiredExtensions() const;
		std::unique_ptr<Window> open(uint32_t w, uint32_t h, const std::string& title) const;

};


class Window {

	private:

		bool capture = false;
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
		void setMouseCapture(bool capture);

	public:

		~Window();

		GLFWwindow* getHandle() const;
		void poll() const;
		bool shouldClose() const;
		void getFramebufferSize(int* width, int* height) const;

		InputDispatcher& getInputDispatcher();

		bool isKeyPressed(int key) const;
		bool isButtonPressed(int button) const;
		glm::vec2 getCursor() const;

};
