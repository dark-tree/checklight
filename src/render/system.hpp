#pragma once

#include "external.hpp"
#include "renderer.hpp"
#include "application.hpp"
#include "window.hpp"
#include "asset/obj.hpp"
#include "shared/singleton.hpp"

class RenderCommander;
class RenderMesh;
class RenderObject;
class RenderModel;

class RenderSystem : public Renderer {

	private:

		/// Import materials for an OBJ file specified by the path
		static std::map<std::string, std::shared_ptr<ObjMaterial>> importMaterials(const std::string& path);

		int frame_count = 0, frame_rate = 0;
		std::chrono::steady_clock::time_point previous = std::chrono::steady_clock::now();

	public:

		static inline Singleton<RenderSystem> system;
		static SingletonGuard<RenderSystem> init(ApplicationParameters& parameters);

	public:

		RenderSystem(ApplicationParameters& parameters);

		/**
		 * Convert a Render Meshes into a Render Model, you probably don't need to use this,
		 * loading a OBJ automatically creates a Render Model.
		 */
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
		 * but needs to be called AT LEAST when window size changes to account for the aspect ratio
		 *
		 * @note This call also updates the previous projection matrix to the current projection matrix
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
		 * @note This call also updates the previous view matrix to the current view matrix
		 *
		 * @param eye     The position of the "eye" in 3D world space
		 * @param facing  Directional vector that specific in which way the camera looks.
		 */
		void setViewMatrix(glm::vec3 eye, glm::vec3 facing);

		/**
		 * Create new RenderObject, each delegate represents one object in the world
		 * each game object can be made from many render objects (delegates)
		 * each delegate contains an affine transformation matrix you can access and modify to move the object
		 * in the world space.
		 */
		std::shared_ptr<RenderObject> createRenderObject();

		/**
		 * Import an OBJ file and return a list of models loaded from it
		 */
		std::vector<std::shared_ptr<RenderModel>> importObj(const std::string& path);

		/**
		 * Get asset manager, this class can be used to request many resources
		 * used during rendering (sprites, fonts)
		 */
		AssetLoader& getAssetLoader();

		/**
		 * Get the immediate style GUI/Debug Renderer,
		 * do note that the performance of this system is very low!
		 */
		ImmediateRenderer& getImmediateRenderer();

		/**
		 * Get the current render parameters
		 */
		RenderParameters& getParameters();

		/**
		 * Returns current approximate framerate
		 * as the number of frames emitted per seconds
		 */
		int getFrameRate() const;

		/**
		 * Render the next frame, all rendering should happen inside this call
		 */
		void draw() override;
};
