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

		void setProjectionMatrix(glm::mat4 projection);
		void setViewMatrix(glm::mat4 view);

	public:

		static std::unique_ptr<RenderSystem> system;
		static void init(ApplicationParameters& parameters);

	public:

		RenderSystem(ApplicationParameters& parameters);

		/**
		 * Commanders are used to command the GPU to perform actions, like
		 * data transfers and the like. This returns a transient commander.
		 * A transient commander is a commander that once completed will cause the CPU
		 * to wait for the GPU to complete the requested operations. This is perfect for
		 * uploading vertex data before entering the render loop.
		 */
		std::unique_ptr<RenderCommander> createTransientCommander();

		/**
		 * Create new empty geometry mesh that can later be drawn
		 * once filled with vertex (and possibly index) data.
		 */
		std::shared_ptr<RenderMesh> createMesh();

		/**
		 * Specifies the perspective used, this should most likely stay constant
		 * but needs to be called AT LEAST one window size changes to account for the aspect ratio
		 *
		 * @note This call modifies an uniform, remember to call updateUniforms() after it!
		 *
		 * @param fov  Camera field of view
		 * @param near The closest anything can be to the camera and render
		 * @param far  The furthest anything can be from the camera and render
		 */
		void setProjectionMatrix(float fov, float near, float far);

		/**
		 * Specifies the view used, this should be called ever frame
		 * to account for the camera movement.
		 *
		 * @note This call modifies an uniform, remember to call updateUniforms() after it!
		 *
		 * @param eye     The position of the "eye" in 3D world space
		 * @param facing  Directional vector that specific in which way the camera looks.
		 */
		void setViewMatrix(glm::vec3 eye, glm::vec3 facing);

		/**
		 * After calling setProjectionMatrix() and/or setViewMatrix() this method must be called
		 * at least once to send the updated uniforms to the GPU.
		 */
		void updateUniforms();

		/**
		 * Changes the currently used material, this can be a VERY slow operation,
		 * so try to avoid needlessly calling it (for example, try to group
		 * all meshes that use the same material and draw them one after another)
		 *
		 * @note This currently does nothing.
		 */
		void bindMaterial();

		/**
		 * Changes the currently used mesh, this can be a QUITE slow operation,
		 * so try to avoid needlessly calling it (for example, try to group
		 * all object that use the same mesh and draw them one after another)
		 *
		 * @param mesh The mesh to bind
		 */
		void bindMesh(std::shared_ptr<RenderMesh>& mesh);

		/**
		 * Draws the bound mesh using the bound material at the given position
		 *
		 * @param model Model materix to use
		 */
		void draw(glm::mat4 model);

};
