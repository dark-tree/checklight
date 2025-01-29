#pragma once

#include "external.hpp"
#include "renderer.hpp"
#include "application.hpp"
#include "window.hpp"

class RenderCommander;
class RenderMesh;

class RenderSystem : public Renderer {

	private:

		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;

	public:

		static std::unique_ptr<RenderSystem> system;
		static void init(ApplicationParameters& parameters);

	public:

		RenderSystem(ApplicationParameters& parameters);

		std::unique_ptr<RenderCommander> createTransientCommander();
		std::shared_ptr<RenderMesh> createMesh();

		void setProjectionMatrix(glm::mat4 projection);
		void setViewMatrix(glm::mat4 projection);
		void setModelMatrix(glm::mat4 projection);

		void drawMesh(std::shared_ptr<RenderMesh>& mesh);

};
