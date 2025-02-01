#pragma once

#include "external.hpp"

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

		GLFWwindow* handle;

		static void glfwKeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
		static void glfwButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods);
		static void glfwScrollCallback(GLFWwindow* glfw_window, double x_scroll, double y_scroll);

		friend class WindowSystem;
		Window(uint32_t w, uint32_t h, std::string title);

	public:

		~Window();

		GLFWwindow* getHandle() const;
		void poll() const;
		bool shouldClose() const;
		void getFramebufferSize(int* width, int* height) const;

		bool isKeyPressed(int key) const;
		bool isButtonPressed(int button) const;
		void getCursor(double* x, double* y) const;
		void setMouseCapture(bool capture);

};
