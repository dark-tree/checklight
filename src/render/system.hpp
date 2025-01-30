#pragma once

#include "external.hpp"
#include "renderer.hpp"
#include "application.hpp"
#include "window.hpp"

class RenderCommander;
class RenderMesh;

class RenderSystem : public Renderer {

	private:

		std::shared_ptr<RenderMesh> mesh;
		SceneUniform uniforms;

	public:

		static std::unique_ptr<RenderSystem> system;
		static void init(ApplicationParameters& parameters);

	public:

		RenderSystem(ApplicationParameters& parameters);

		std::unique_ptr<RenderCommander> createTransientCommander();
		std::shared_ptr<RenderMesh> createMesh();

		void setProjectionMatrix(glm::mat4 projection);
		void setProjectionMatrix(float fov, float near_plane, float far_plane);
		void setViewMatrix(glm::mat4 projection);
		void updateUniforms();

		void bindMaterial();
		void bindMesh(std::shared_ptr<RenderMesh>& mesh);

		void draw(glm::mat4 model);

};
