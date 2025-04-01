
#include <gui/context.hpp>
#include <gui/widget/panel.hpp>
#include <shared/args.hpp>
#include <shared/logger.hpp>

#include "render/render.hpp"
#include "input/input.hpp"
#include "engine/engine.hpp"
#include "gui/gui.hpp"

static void entry(Args& args) {
	ApplicationParameters parameters;
	parameters.setName("My Checklight Game!");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);
	Models::init();

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();

	auto models = system.importObj("assets/models/checklight.obj");

	BoardManager manager(window);

	std::shared_ptr<Pawn> pawn = std::make_shared<Pawn>();

	auto pawnThatRendersTheCube = std::make_shared<SpatialPawn>();
	auto pawnThatRendersTheSphere = std::make_shared<SpatialPawn>();

	pawnThatRendersTheCube->setPosition({-2,1,-2});

	pawnThatRendersTheSphere->setPosition({2,1,2});

	std::shared_ptr<Component> renderSphere = std::make_shared<RenderComponent>(Models::SPHERE);
	std::shared_ptr<Component> renderCube = std::make_shared<RenderComponent>(Models::CUBE);


	pawnThatRendersTheSphere->addComponent(renderSphere);
	pawnThatRendersTheCube->addComponent(renderCube);

	pawn->setName("Test");

	std::shared_ptr<Board> sp = manager.getCurrentBoard().lock();

	sp->addPawnToRoot(pawn);
	sp->addPawnToRoot(pawnThatRendersTheCube);
	sp->addPawnToRoot(pawnThatRendersTheSphere);


	std::vector<std::shared_ptr<RenderObject>> objects;



	for (auto& model : models) {
		auto object = system.createRenderObject();
		object->setMatrix(glm::identity<glm::mat4x3>());
		object->setModel(model);
		objects.push_back(object);
	}

	uint64_t a = 0;
	while (!window.shouldClose()) {
		window.poll();
		a++;

		pawnThatRendersTheCube->setRotation(glm::rotate(pawnThatRendersTheCube->getRotation(),0.01f,{1.0f,1.0f,1.0f}));

		//physics update before rendering
		manager.updateCycle();
		std::shared_ptr<Board> current_board = manager.getCurrentBoard().lock();


		//drawUserInterface(system.getImmediateRenderer(), system.width(), system.height());

		// update uniforms
		// do this once at the beginning of frame rendering
		system.setProjectionMatrix(40.0f, 0.001f, 10000.0f);
		system.setViewMatrix(current_board->getCamPos(), current_board->getCamForward());

		// render the scene
		system.draw();
	}

	system.wait();
	Models::terminate();//temporary

	for (auto& object : objects) {
		object.reset();
	}

	for (auto& model : models) {
		system.closeModel(model);
		model.reset();
	}

	RenderSystem::system.reset();

}

int main(int argc, const char* argv[]) {

	Args args {argc, argv};

	std::string path = std::filesystem::current_path().generic_string();
	out::info("Current working directory: %s", path.c_str());

	if (args.has("--verbose")) {
		out::logger.setLogLevelMask(Logger::LEVEL_VERBOSE);
	}

	entry(args);

	Models::terminate();//temporary

	// Try to close all engine systems before the logger shuts itself of
	RenderSystem::system.reset();
	return 0;
}
