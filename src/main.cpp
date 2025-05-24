
#include <gui/context.hpp>
#include <gui/debug/render.hpp>
#include <gui/theme/dark.hpp>
#include <gui/theme/light.hpp>
#include <gui/widget/panel.hpp>
#include <gui/widget/root.hpp>
#include <gui/widget/text.hpp>
#include <shared/args.hpp>
#include <shared/logger.hpp>
#include "sound/sound.hpp"
#include "render/render.hpp"
#include "input/input.hpp"
#include "engine/engine.hpp"
#include "gui/gui.hpp"
#include "engine/entity/component/matrixAnimation.hpp"
#include "glm/gtc/noise.hpp"

static void entry(Args& args) {

	// Basic information about the program being run
	ApplicationParameters parameters;
	parameters.setName("My Checklight Game!");
	parameters.setDimensions(1200, 800);

	// Open the window and start the vulkan renderer
	auto guard = RenderSystem::init(parameters);
	Models::init();

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();
	RenderParameters& options = system.getParameters();

	// Configure the renderer
	options.setAmbientLight(glm::vec3(0.0, 0.0, 0.0));
	options.setDenoise(true);
	options.setShadows(true);
	options.setGISamples(1);
	options.setPortalGIEnable(false);

	// Simple overlay to play with render options
	AutoTheme theme {};
	auto context = RenderSystemOverlay::create(theme);

	//window.getInputDispatcher().registerListener(std::make_shared<DebugInputListener>());
	window.getInputDispatcher().registerListener(std::dynamic_pointer_cast<InputListener>(context));
	auto models = system.importObj("assets/models/checklight.obj");
	auto cube = system.importObj("assets/models/cube.obj");

	BoardManager manager(std::shared_ptr<InputDispatcher>(&window.getInputDispatcher()));

	std::shared_ptr<Pawn> pawn = std::make_shared<Pawn>();

	auto pawnThatRendersTheCube = std::make_shared<SpatialPawn>();
	auto pawnThatRendersTheSphere = std::make_shared<SpatialPawn>();

	pawnThatRendersTheCube->setPosition({-2,1,-2});

	pawnThatRendersTheSphere->setPosition({2,1,2});

	pawnThatRendersTheSphere->createComponent<RenderComponent>(Models::SPHERE);
	pawnThatRendersTheSphere->createComponent<MatrixAnimation>(MatrixAnimation::TRANSLATE);

	pawnThatRendersTheCube->createComponent<RenderComponent>(Models::CUBE);
	pawnThatRendersTheCube->createComponent<MatrixAnimation>(MatrixAnimation::ROTATE);

	pawn->setName("Test");

	std::shared_ptr<Board> sp = manager.getCurrentBoard().lock();

	sp->addPawnToRoot(pawn);
	sp->addPawnToRoot(pawnThatRendersTheCube);
	sp->addPawnToRoot(pawnThatRendersTheSphere);
    std::shared_ptr<SpatialPawn> camera_pawn = static_pointer_cast<SpatialPawn>(sp->getTree().findByName("Main Camera"));
    camera_pawn->setPosition({10, 5, 10});
    std::shared_ptr<Camera> camrera = static_pointer_cast<Camera>(camera_pawn->getComponents()[0]);

	std::vector<std::shared_ptr<RenderObject>> objects;

	for (auto& model : models) {
		auto object = system.createRenderObject();
		object->setMatrix(glm::identity<glm::mat4x3>());
		object->setModel(model);
		objects.push_back(object);
	}

	for (auto& model : cube) {
		auto object = system.createRenderObject();
		object->setMatrix(glm::translate(glm::identity<glm::mat4>(), glm::vec3(4, 0, 4)));
		object->setModel(model);

		glm::mat4 portal = glm::translate(glm::identity<glm::mat4>(), glm::vec3(-4, 8, 4));
		portal = glm::rotate(portal, glm::radians(90.0f), glm::vec3(0, 0, 1));
		object->setPortal(portal);
		objects.push_back(object);
	}

	system.getParameters().setAmbientLight(glm::vec3(0.0, 0.0, 0.0));
	system.getParameters().setDenoise(true);
	system.getParameters().setShadows(true);
	system.getParameters().setGISamples(1);

	system.getLightManager().createDirectionalLight(
		{0.0, 3.5, -1.0},
		{1.0, 1.0, 1.0},
		1.5,
		true
	);

	auto point_light = system.getLightManager().createPointLight(
		{3.0, 2.0, 18.0},
		{0.0, 0.0, 1.0},
		50.0,
		true
	);

	SoundListener::setPosition(0, 0, 0);
	SoundManager& sm = SoundManager::getInstance();
	auto sso = std::make_shared<SoundSourceObject>();
	sm.addSource(sso);
	sm.createSoundClipAndAddToSourceObject("assets/sounds/testOGG.ogg", sso);
	sso->setPosition(0.0f, 4.8f, 0.0f);
	sso->setMaxDistance(10.0f);
	sso->setReferenceDistance(1.0f);
	sso->setRolloffFactor(1.0f);
	sso->setMinGain(0.0f);
	sso->setMaxGain(1.0f);
	SoundListener::setDistanceModel(AL_LINEAR_DISTANCE);
	//window.getInputDispatcher().registerListener(std::make_shared<DebugInputListener>());
	while (!window.shouldClose()) {
		window.poll();

		//physics update before rendering
		manager.updateCycle();
		std::shared_ptr<Board> current_board = manager.getCurrentBoard().lock();

		// draw render system overlay
		context->draw(system.getImmediateRenderer());

		// draw framerate onto the screen
		const int fps = system.getFrameRate();
		ImmediateRenderer& immediate = system.getImmediateRenderer();
		immediate.setFontSize(30);
		immediate.setFill(0, 0, 0);
		immediate.setTextBox(OFF);
		immediate.setWrapping(false);
		immediate.setTextAlignment(HorizontalAlignment::RIGHT);
		immediate.setTextAlignment(VerticalAlignment::TOP);
		immediate.drawString2D(system.width() - 10, 10, "FPS: " + std::to_string(fps));

		// update uniforms, do this once at the beginning of frame rendering
		system.setProjectionMatrix(65.0f, 0.01f, 1000.0f);
		system.setViewMatrix(current_board->getCamPos(), current_board->getCamForward());

		// update lights
		point_light->vector = glm::vec3(3.0, 2.0, 18.0 * sin(glfwGetTime() / 8));
		point_light->color = glm::vec3(sin(glfwGetTime() / 2) * 0.5 + 0.5, sin(glfwGetTime() / 3 + 2) * 0.5 + 0.5, sin(glfwGetTime() / 5 + 4) * 0.5 + 0.5);
		system.getLightManager().flush();

		// render the scene
		system.draw();
		sm.playSound(sso);
		SoundListener::setPosition(current_board->getCamPos());
		SoundListener::setOrientation(current_board->getCamForward(),glm::vec3(0.0f,1.0f,0.0f));
	}

	system.wait();
	Models::terminate();

}

int main(int argc, const char* argv[]) {

	Args args {argc, argv};

	std::string path = std::filesystem::current_path().generic_string();
	out::info("Current working directory: %s", path.c_str());

	if (args.has("--verbose")) {
		out::logger.setLogLevelMask(Logger::LEVEL_VERBOSE);
	}

	entry(args);

	return 0;

}
