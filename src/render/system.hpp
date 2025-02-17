#pragma once

#include "external.hpp"
#include "renderer.hpp"
#include "application.hpp"
#include "window.hpp"

class RenderCommander;
class RenderMesh;
class RenderObject;
class RenderModel;

class RenderSystem : public Renderer {

	public:

		static std::unique_ptr<RenderSystem> system;
		static void init(ApplicationParameters& parameters);

	public:

		RenderSystem(ApplicationParameters& parameters);

		std::vector<std::shared_ptr<RenderModel>> createRenderModels(std::vector<std::shared_ptr<RenderMesh>> meshes);

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
		 * Create new InstanceDelegate, each delegate represents one object int the world
		 * each game object can be made from many render objects (delegates)
		 * each delegate contains an affine transformation matrix you can access and modify to move the object
		 * in the world space.
		 */
		std::shared_ptr<RenderObject> createRenderObject();

};
