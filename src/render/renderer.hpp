#pragma once

#include "external.hpp"
#include "application.hpp"
#include "window.hpp"

class Renderer {

	private:

		WindowSystem windows;
		std::unique_ptr<Window> window;

		VkInstance instance;

	private:

		void createInstance();

	public:

		Renderer(ApplicationParameters& parameters);
		~Renderer();

		Window& getWindow() const;

};
