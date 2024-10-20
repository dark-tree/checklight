#pragma once

#include "external.hpp"
#include "renderer.hpp"
#include "application.hpp"
#include "window.hpp"

class RenderSystem {

	private:

		static std::unique_ptr<Renderer> renderer;

	public:

		/**
		 * TODO
		 */
		static Renderer& getRenderer();

		/**
		 * TODO
		 */
		static Window& getWindow();

		/**
		 * TODO
		 */
		static void initialize(ApplicationParameters& parameters);

		/**
		 * TODO
		 */
		static void terminate();

};
