#pragma once

#include "external.hpp"
#include "renderer.hpp"
#include "application.hpp"
#include "window.hpp"

class RenderCommander;
class RenderMesh;

class RenderSystem : public Renderer {

	private:

		std::vector<std::shared_ptr<RenderMesh>> meshes;
		void drawFrame(RenderFrame& frame, CommandRecorder& recorder, uint32_t image) override;

	public:

		static std::unique_ptr<RenderSystem> system;
		static void init(ApplicationParameters& parameters);

	public:

		RenderSystem(ApplicationParameters& parameters);

		void close();

		std::unique_ptr<RenderCommander> createTransientCommander();
		std::shared_ptr<RenderMesh> createMesh();

		void addForRendering(std::shared_ptr<RenderMesh>& mesh);

};
