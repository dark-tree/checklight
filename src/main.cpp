#include <engine/entity/component/sound.hpp>
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
	parameters.setDimensions(1500, 1000);

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
	AutoTheme theme{};
	auto context = RenderSystemOverlay::create(theme);

	//window.getInputDispatcher().registerListener(std::make_shared<DebugInputListener>());
	window.getInputDispatcher().registerListener(context);
	auto models = system.importObj("assets/models/checklight.obj");
	auto cube = system.importObj("assets/models/cube.obj");

	auto dispacher = std::make_shared<InputDispatcher>();
	window.getInputDispatcher().registerListener(dispacher);
	BoardManager manager(dispacher);
	manager.setGravity(glm::vec3(0, -10, 0));

	std::shared_ptr<Board> sp = manager.getCurrentBoard().lock();

	{
		auto invisible_pawn = std::make_shared<SpatialPawn>();
		invisible_pawn->setPosition(glm::vec3(0, 10, 0));
		invisible_pawn->createComponent<SoundComponent>("assets/sounds/4.ogg");
		sp->addPawnToRoot(invisible_pawn);
	}

	auto cube_1 = std::make_shared<SpatialPawn>();
	cube_1->setPosition({-2, 1, -2});
	cube_1->createComponent<RenderComponent>(Models::CUBE);
	cube_1->createComponent<MatrixAnimation>(MatrixAnimation::ROTATE);
	cube_1->createComponent<SoundComponent>("assets/sounds/2.ogg");
	sp->addPawnToRoot(cube_1);

	auto sphere = std::make_shared<SpatialPawn>();
	sphere->setPosition({2, 1, 2});
	sphere->createComponent<RenderComponent>(Models::SPHERE);
	sphere->createComponent<MatrixAnimation>(MatrixAnimation::TRANSLATE);
	sphere->createComponent<SoundComponent>("assets/sounds/5.ogg");
	sp->addPawnToRoot(sphere);
    {
        auto cube_2 = std::make_shared<SpatialPawn>();
        cube_2->setPosition({30, 1.1, 5});
        cube_2->createComponent<RenderComponent>(Models::CUBE);
        cube_2->setRotation(rotate(glm::quat(), {0, 0, 0}));
        cube_2->createComponent<SoundComponent>("assets/sounds/1.ogg");
        auto pc = cube_2->createComponent<PhysicsComponent>();
        pc->setVelocity({0, 10, 0});
        pc->setAngularVelocity({0, 0, 0});
        pc->setGravityScale({0, 1, 0});
        pc->getMaterial().coefficient_of_restitution = -0.2f;
        sp->addPawnToRoot(cube_2);
    }
	{
		auto cube_2 = std::make_shared<SpatialPawn>();
		cube_2->setPosition({30, 1.1, 10});
		cube_2->createComponent<RenderComponent>(Models::CUBE);
		cube_2->setRotation(rotate(glm::quat(), {0, 0, 0}));
		cube_2->createComponent<SoundComponent>("assets/sounds/1.ogg");
		auto pc = cube_2->createComponent<PhysicsComponent>();
		pc->setVelocity({0, 10, 0});
		pc->setAngularVelocity({0, 0, 0});
		pc->setGravityScale({0, 1, 0});
        pc->getMaterial().coefficient_of_restitution = 0.5f;
		sp->addPawnToRoot(cube_2);
	}
    {
        auto cube_2_5 = std::make_shared<SpatialPawn>();
        cube_2_5->setPosition({30, 1.1, 15});
        cube_2_5->createComponent<RenderComponent>(Models::CUBE);
        cube_2_5->setRotation(rotate(glm::quat(), {0, 0, 0}));
        cube_2_5->createComponent<SoundComponent>("assets/sounds/1.ogg");
        auto pc = cube_2_5->createComponent<PhysicsComponent>();
        pc->setVelocity({0, 10, 0});
        pc->setAngularVelocity({0, 0, 0});
        pc->setGravityScale({0, 1, 0});
        pc->getMaterial().coefficient_of_restitution = 1.f;
        sp->addPawnToRoot(cube_2_5);
    }
	{
		auto cube_3 = std::make_shared<SpatialPawn>();
		cube_3->setPosition({30, 1.1, 20});
		cube_3->createComponent<RenderComponent>(Models::CUBE);
		cube_3->createComponent<SoundComponent>("assets/sounds/3.ogg");
		auto pc = cube_3->createComponent<PhysicsComponent>();
		pc->setVelocity({0, 10, 0});
        pc->getMaterial().coefficient_of_restitution = 1.8f;
		sp->addPawnToRoot(cube_3);
	}
	{
		auto floor = std::make_shared<SpatialPawn>();
		floor->setPosition({0, -1000, 0});
		auto pc = floor->createComponent<PhysicsComponent>();
		pc->setGravityScale({0, 0, 0});
		pc->setVelocity({0, 0, 0});
		pc->setAngularVelocity({0, 0, 0});
		pc->getCollider().setVertices({
			{-1000.f, -1000.f, 1000.f},
			{1000.f, -1000.f, 1000.f},
			{1000.f, 1000.f, 1000.f},
			{-1000.f, 1000.f, 1000.f},
			{1000.f, -1000.f, -1000.f},
			{-1000.f, -1000.f, -1000.f},
			{-1000.f, 1000.f, -1000.f},
			{1000.f, 1000.f, -1000.f}
		});
		sp->addPawnToRoot(floor);
	}

	sp->addPawnToRoot(cube_1);
	sp->addPawnToRoot(sphere);

	auto camera_pawn = static_pointer_cast<SpatialPawn>(sp->getTree().findByName("Main Camera"));
	camera_pawn->setPosition({10, 5, 10});

	std::vector<std::shared_ptr<RenderObject>> objects;

	for (auto& model: models) {
		auto object = system.createRenderObject();
		object->setMatrix(glm::identity<glm::mat4x3>());
		object->setModel(model);
		objects.push_back(object);
	}

	for (auto& model: cube) {
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

		// DEBUG

		immediate.setBillboardTarget(current_board->getCamPos());
		current_board->getTree().getRoot()->debugDraw(immediate);

		// update uniforms, do this once at the beginning of frame rendering
		system.setProjectionMatrix(65.0f, 0.01f, 1000.0f);
		system.setViewMatrix(current_board->getCamPos(), current_board->getCamForward());

		// update lights
		point_light->vector = glm::vec3(3.0, 2.0, 18.0 * sin(glfwGetTime() / 8));
		point_light->color = glm::vec3(sin(glfwGetTime() / 2) * 0.5 + 0.5, sin(glfwGetTime() / 3 + 2) * 0.5 + 0.5,
		                               sin(glfwGetTime() / 5 + 4) * 0.5 + 0.5);
		system.getLightManager().flush();

		// render the scene
		system.draw();
	}

	system.wait();
	Models::terminate();
}

int main(int argc, const char* argv[]) {
	Args args{argc, argv};

	std::string path = std::filesystem::current_path().generic_string();
	out::info("Current working directory: %s", path.c_str());

	if (args.has("--verbose")) {
		out::logger.setLogLevelMask(Logger::LEVEL_VERBOSE);
	}

	entry(args);

	return 0;
}
