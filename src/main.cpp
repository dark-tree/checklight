#include <engine/entity/component/sound.hpp>
#include <shared/args.hpp>
#include <shared/logger.hpp>
#include "gui/gui.hpp"
#include "render/render.hpp"
#include "input/input.hpp"
#include "engine/engine.hpp"
#include "engine/entity/component/matrixAnimation.hpp"
#include "gui/debug/render.hpp"
#include "engine/entity/component/movement.hpp"

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
	window.getInputDispatcher().registerListener(context, 1);
	auto models = system.importObj("assets/models/checklight.obj");
	auto cube = system.importObj("assets/models/cube.obj");
    auto wall = system.importObj("assets/models/Wall.obj");

	auto dispacher = std::make_shared<InputDispatcher>();
	window.getInputDispatcher().registerListener(dispacher, 2);
	BoardManager manager(dispacher);
	manager.setGravity(glm::vec3(0, -10, 0));

	std::shared_ptr<Board> sp = manager.getCurrentBoard().lock();
	{
		auto invisible_pawn = std::make_shared<SpatialPawn>();
		invisible_pawn->setPosition(glm::vec3(0, 10, 0));
		//invisible_pawn->createComponent<SoundComponent>("assets/sounds/4.ogg");
		//sp->addPawnToRoot(invisible_pawn);
	}
	// ODTAD MOZNA USUNAC
    {
		auto cube_1 = std::make_shared<SpatialPawn>();
		cube_1->setPosition({-2, 1, -2});
		cube_1->createComponent<RenderComponent>(Models::getShape(Models::CUBE));
		cube_1->createComponent<MatrixAnimation>(MatrixAnimation::ROTATE);
		cube_1->createComponent<SoundComponent>("assets/sounds/2.ogg");
		sp->addPawnToRoot(cube_1);
	}
	{
		auto cube_2 = std::make_shared<SpatialPawn>();
		cube_2->setPosition({30, 1.1, 5});
		cube_2->createComponent<RenderComponent>(Models::getShape(Models::CUBE));
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
		auto cube_3 = std::make_shared<SpatialPawn>();
		cube_3->setPosition({30, 1.1, 10});
		cube_3->createComponent<RenderComponent>(Models::getShape(Models::CUBE));
		cube_3->setRotation(rotate(glm::quat(), {0, 0, 0}));
		cube_3->createComponent<SoundComponent>("assets/sounds/1.ogg");
		auto pc = cube_3->createComponent<PhysicsComponent>();
		pc->setVelocity({0, 10, 0});
		pc->setAngularVelocity({0, 0, 0});
		pc->setGravityScale({0, 1, 0});
		pc->getMaterial().coefficient_of_restitution = 0.5f;
		sp->addPawnToRoot(cube_3);
	}
	{
		auto cube_4 = std::make_shared<SpatialPawn>();
		cube_4->setPosition({30, 1.1, 15});
		cube_4->createComponent<RenderComponent>(Models::getShape(Models::CUBE));
		cube_4->setRotation(rotate(glm::quat(), {0, 0, 0}));
		cube_4->createComponent<SoundComponent>("assets/sounds/1.ogg");
		auto pc = cube_4->createComponent<PhysicsComponent>();
		pc->setVelocity({0, 10, 0});
		pc->setAngularVelocity({0, 0, 0});
		pc->setGravityScale({0, 1, 0});
		pc->getMaterial().coefficient_of_restitution = 1.f;
		sp->addPawnToRoot(cube_4);
	}
	{
		auto cube_5 = std::make_shared<SpatialPawn>();
		cube_5->setPosition({30, 1.1, 20});
		cube_5->createComponent<RenderComponent>(Models::getShape(Models::CUBE));
		cube_5->createComponent<SoundComponent>("assets/sounds/3.ogg");
		auto pc = cube_5->createComponent<PhysicsComponent>();
		pc->setVelocity({0, 10, 0});
		pc->getMaterial().coefficient_of_restitution = 1.8f;
		sp->addPawnToRoot(cube_3);
	}
	{
		auto sphere = std::make_shared<SpatialPawn>();
		sphere->setPosition({35, 1.1, 15});
		sphere->createComponent<RenderComponent>(Models::getShape(Models::SPHERE));
		sphere->createComponent<MatrixAnimation>(MatrixAnimation::TRANSLATE);
		//sphere->createComponent<SoundComponent>("assets/sounds/5.ogg");
		sp->addPawnToRoot(sphere);
	}
	// DOTAD MOZNA USUNAC
	{
		auto floor = std::make_shared<SpatialPawn>();
		floor->setPosition({0, -1000, 0});
		auto pc = floor->createComponent<PhysicsComponent>();
		pc->setGravityScale({0, 0, 0});
		pc->setVelocity({0, 0, 0});
		pc->setAngularVelocity({0, 0, 0});
		pc->setVertices({
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
        pc1 = pc;
	}

	// sp->addPawnToRoot(cube_1);
	// sp->addPawnToRoot(sphere);

    std::shared_ptr<SpatialPawn> wallObj0;
    for (int i = 0; i < 2; i++) {
        auto wallObj = std::make_shared<SpatialPawn>();
        wallObj->setPosition({i * 4, 2.501, 0}); //hiper dziwne, pokazać skajowi
        wallObj->createComponent<RenderComponent>("assets/models/Wall.obj");
        auto pc = wallObj->createComponent<PhysicsComponent>();
        pc->setVelocity({0, 0, 0});
        pc->setGravityScale({0, 0, 0});
        pc->setVertices({
            {-0.2f, -2.5f, 500.f},
            {0.2f, -2.5f, 500.f},
            {0.2f, 2.5f, 500.f},
            {-0.2f, 2.5f, 500.f},
            {0.2f, -2.5f, -500.f},
            {-0.2f, -2.5f, -500.f},
            {-0.2f, 2.5f, -500.f},
            {0.2f, 2.5f, -500.f}
        });
        pc->setMass(999999999999999);
        pc->setStatic(true);
        sp->addPawnToRoot(wallObj);
        wallObj0 = wallObj;
    }

    {
        auto cube_3 = std::make_shared<SpatialPawn>();
        cube_3->setPosition({-35, 2.5, 499});
        cube_3->createComponent<RenderComponent>(Models::getShape(Models::CUBE));
        auto pc = cube_3->createComponent<PhysicsComponent>();
        pc->setVelocity({10, 10, 0});
        pc->getMaterial().coefficient_of_restitution = 1.f;
        pc->getMaterial().coefficient_of_friction = -0.2f;
        pc->setGravityScale({0, 1, 0});
        sp->addPawnToRoot(cube_3);
    }

    auto roofObj = std::make_shared<SpatialPawn>();
    roofObj->setPosition({2, 5,  0});
    roofObj->createComponent<RenderComponent>("assets/models/Roof.obj");
    sp->addPawnToRoot(roofObj);

    auto overpassObj = std::make_shared<SpatialPawn>();
    overpassObj->setPosition({2, 5,  495});
    overpassObj->createComponent<RenderComponent>("assets/models/Overpass.obj");
    sp->addPawnToRoot(overpassObj);

    auto overpassObj2 = std::make_shared<SpatialPawn>();
    overpassObj2->setPosition({2, 5,  -485});
    overpassObj2->createComponent<RenderComponent>("assets/models/Overpass.obj");
    sp->addPawnToRoot(overpassObj2);

    //wall
    for (int i = 0; i < 2; i++) {
        auto wallObj = std::make_shared<SpatialPawn>();
        wallObj->setPosition({-300 + i * 604, 1.5, 495 + i * 0.0001});
        wallObj->createComponent<RenderComponent>("assets/models/Fence.obj");
        auto pc = wallObj->createComponent<PhysicsComponent>();
        pc->setVelocity({0, 0, 0});
        pc->setGravityScale({0, 0, 0});
        pc->setVertices({
            {-300.f, -1.5f, 0.2f},
            {300.f, -1.5f, 0.2f},
            {300.f, 1.5f, 0.2f},
            {-300.f, 1.5f, 0.2f},
            {300.f, -1.5f, -0.2f},
            {-300.f, -1.5f, -0.2f},
            {-300.f, 1.5f, -0.2f},
            {300.f, 1.5f, -0.2f}
        });
        pc->setMass(999999999999999);
        sp->addPawnToRoot(wallObj);
    }

    //wall + portal
    for (int i = 0; i < 2; i++) {
        auto wallObj = std::make_shared<SpatialPawn>();
        wallObj->setPosition({-304 + i * 612, 4.5, 495 + i * 0.0001});
        auto object = wallObj->createComponent<RenderComponent>("assets/models/Fence1.obj");


        glm::mat4 portal = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, -980));
		//portal = glm::rotate(portal, glm::radians(90.0f), glm::vec3(0, 1, 0));
		object->getRenderObject()->setPortal(portal);

        sp->addPawnToRoot(wallObj);
    }

    //wall other side
    for (int i = 0; i < 2; i++) {
        auto wallObj = std::make_shared<SpatialPawn>();
        wallObj->setPosition({-300 + i * 604, 1.5, -485 + i * 0.0001});
        wallObj->createComponent<RenderComponent>("assets/models/Fence.obj");
        auto pc = wallObj->createComponent<PhysicsComponent>();
        pc->setVelocity({0, 0, 0});
        pc->setGravityScale({0, 0, 0});
        pc->setVertices({
            {-300.f, -1.5f, 0.2f},
            {300.f, -1.5f, 0.2f},
            {300.f, 1.5f, 0.2f},
            {-300.f, 1.5f, 0.2f},
            {300.f, -1.5f, -0.2f},
            {-300.f, -1.5f, -0.2f},
            {-300.f, 1.5f, -0.2f},
            {300.f, 1.5f, -0.2f}
        });
        pc->setMass(999999999999999);
        sp->addPawnToRoot(wallObj);
    }

    //wall + portal other side
//    for (int i = 0; i < 2; i++) {
//        auto wallObj = std::make_shared<SpatialPawn>();
//        wallObj->setPosition({-300 + i * 604, 4.5, -495 + i * 0.0001});
//        auto object = wallObj->createComponent<RenderComponent>("assets/models/Fence1.obj");
//
//
////        glm::mat4 portal = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, 989));
////        //portal = glm::rotate(portal, glm::radians(90.0f), glm::vec3(0, 1, 0));
////        object->getRenderObject()->setPortal(portal);
//
//        sp->addPawnToRoot(wallObj);
//    }

//	sp->addPawnToRoot(cube_1);
//	sp->addPawnToRoot(sphere);

    auto playerObj = std::make_shared<SpatialPawn>();
    playerObj->setPosition({-10, 4, 505});
    auto pc = playerObj->createComponent<PhysicsComponent>();
    auto mc = playerObj->createComponent<MovementComponent>();
    pc->setVelocity({0, 0, 0});
    pc->setGravityScale({0, 1, 0});
    pc->setMass(12);
    sp->addPawnToRoot(playerObj);

    dispacher->registerListener(mc);

	auto camera_pawn = static_pointer_cast<SpatialPawn>(sp->getTree().findByName("Main Camera"));
	camera_pawn->setPosition({-10, 4, 505});
    //camera_pawn->setRotation(glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0)));
    static_pointer_cast<Camera>(camera_pawn->getComponents()[0])->setSpeed(0);
    playerObj->addChild(camera_pawn);

	std::vector<std::shared_ptr<RenderObject>> objects;
    //logika z rebase
//    for(auto& model : models) {
//		auto pawn = std::make_shared<SpatialPawn>();
//		pawn->createComponent<RenderComponent>(model);
//		sp->addPawnToRoot(pawn);
//
//		// auto object = system.createRenderObject(false);
//		// object->setMatrix(glm::identity<glm::mat4x3>());
//		// object->setModel(model);
//		// objects.push_back(object);
//	}

    auto object = system.createRenderObject();
    object->setMatrix(glm::identity<glm::mat4x3>());
    object->setModel(models[1]);
    objects.push_back(object);

//	for(auto& model : models) {
//		auto object = system.createRenderObject();
//		object->setMatrix(glm::identity<glm::mat4x3>());
//		object->setModel(model);
//		objects.push_back(object);
//	}
//
//	for(auto& model : cube) {
//		auto object = system.createRenderObject();
//		object->setMatrix(glm::translate(glm::identity<glm::mat4>(), glm::vec3(4, 0, 4)));
//		object->setModel(model);
//
//		glm::mat4 portal = glm::translate(glm::identity<glm::mat4>(), glm::vec3(-4, 8, 4));
//		portal = glm::rotate(portal, glm::radians(90.0f), glm::vec3(0, 0, 1));
//		object->setPortal(portal);
//		objects.push_back(object);
//	}

	system.getParameters().setAmbientLight(glm::vec3(0.0, 0.0, 0.0));
	system.getParameters().setDenoise(true);
	system.getParameters().setShadows(true);
	system.getParameters().setGISamples(1);
    system.getParameters().setPortalGIEnable(true);

	system.getLightManager().createDirectionalLight(
		{2, 3.5, 1.5},
		{1.0, 1.0, 1.0},
		1.5,
		true
	);

    for (int i = 0; i < 8; i++) {
        auto point_light = system.getLightManager().createPointLight(
                {2.0, 4.5, -475 + i * 125},
                {i >= 4 ? 1.0 : 0, ((i + 2) % 4 == 0 || (i + 1) % 4 == 0) ? 1.0 : 0, i % 2 != 0 ? 1.0 : 0},
                20.0,
                true
        );
        printf("%f, %f, %f \n", i >= 4 ? 1.0 : 0, ((i + 2) % 4 == 0 || (i + 1) % 4 == 0) ? 1.0 : 0, i % 2 != 0 ? 1.0 : 0);
    }


    double lastTime;
    double currentTime = glfwGetTime();
    float deltaT;

	//window.getInputDispatcher().registerListener(std::make_shared<DebugInputListener>());
	while(!window.shouldClose()) {
		window.poll();

        lastTime = currentTime;
        currentTime = glfwGetTime();
        deltaT = currentTime - lastTime;

        mc->setDirection(static_pointer_cast<Camera>(camera_pawn->getComponents()[0])->getCamFacing());
        //roofObj->setPosition(roofObj->getPosition() + glm::vec3 {0.001, 0.001, 0.001});
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
//		point_light->vector = glm::vec3(3.0, 2.0, 18.0 * sin(glfwGetTime() / 8));
//		point_light->color = glm::vec3(sin(glfwGetTime() / 2) * 0.5 + 0.5, sin(glfwGetTime() / 3 + 2) * 0.5 + 0.5,
//		                               sin(glfwGetTime() / 5 + 4) * 0.5 + 0.5);
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

	if(args.has("--verbose")) {
		out::logger.setLogLevelMask(Logger::LEVEL_VERBOSE);
	}

	entry(args);

	return 0;
}
