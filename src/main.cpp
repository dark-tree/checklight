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
    RenderParameters &options = system.getParameters();

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

	//game models
    auto models = system.importObj("assets/models/checklight.obj");
    auto cube = system.importObj("assets/models/cube.obj");
    auto wallModel = system.importObj("assets/models/Wall.obj");
	auto roofModel = system.importObj("assets/models/Roof.obj");
	auto gateModel = system.importObj("assets/models/Gate.obj");
	auto overpassModel = system.importObj("assets/models/Overpass.obj");
	auto playerModel = system.importObj("assets/models/Player.obj");
	auto mirrorModel = system.importObj("assets/models/Mirror.obj");
	auto flagModel = system.importObj("assets/models/Flag.obj");
	auto fenceModel = system.importObj("assets/models/Fence.obj");
	auto speakersModel = system.importObj("assets/models/Speakers.obj");
	auto fenceModel1 = system.importObj("assets/models/Fence1.obj");

    auto dispacher = std::make_shared<InputDispatcher>();
    window.getInputDispatcher().registerListener(dispacher, 1);
    BoardManager manager(dispacher);
    manager.setGravity(glm::vec3(0, -10, 0));

	std::shared_ptr<Board> sp = manager.getCurrentBoard().lock();
	{
		auto invisible_pawn = std::make_shared<SpatialPawn>();
		invisible_pawn->setPosition(glm::vec3(0, 10, 0));
	}
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
	}

    for (int i = 0; i < 2; i++) {
        auto wallObj = std::make_shared<SpatialPawn>();
        wallObj->setPosition({i * 4, 2.501, 0}); //hiper dziwne, pokazać skajowi
        wallObj->createComponent<RenderComponent>(wallModel[0]);
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
    }

    {
        auto cube_3 = std::make_shared<SpatialPawn>();
        cube_3->setPosition({-35, 2.5, 503.1});
        cube_3->createComponent<RenderComponent>(Models::getShape(Models::CUBE));
        auto pc = cube_3->createComponent<PhysicsComponent>();
        pc->setVelocity({10, 10, 0});
        pc->getMaterial().coefficient_of_restitution = 1.f;
        pc->getMaterial().coefficient_of_friction = 0;
        pc->setGravityScale({0, 1, 0});
        sp->addPawnToRoot(cube_3);
    }
    {
        auto roofObj = std::make_shared<SpatialPawn>();
        roofObj->setPosition({2, 5.1, 0});
        roofObj->createComponent<RenderComponent>(roofModel[0]);
        sp->addPawnToRoot(roofObj);
        auto pc = roofObj->createComponent<PhysicsComponent>();
        pc->setVelocity({0, 0, 0});
        pc->setGravityScale({0, 0, 0});
        pc->setVertices({
            {-1.f, -0.2f, 500.f},
            {1.f,  -0.2f, 500.f},
            {1.f,  0.2f,  500.f},
            {-1.f, 0.2f,  500.f},
            {1.f,  -0.2f, -500.f},
            {-1.f, -0.2f, -500.f},
            {-1.f, 0.2f,  -500.f},
            {1.f,  0.2f,  -500.f}
        });
        pc->setMass(99999999999);
        pc->setStatic(true);
    }

    for (int k = 0; k < 2; k++) {
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 10; i++) {
                auto fenceObj = std::make_shared<SpatialPawn>();
                fenceObj->setPosition({25 - 50 * j, 1, 498 + i * 6.2 - 1041.8 * k});
                fenceObj->createComponent<RenderComponent>(gateModel[0]);
                sp->addPawnToRoot(fenceObj);
                auto pc = fenceObj->createComponent<PhysicsComponent>();
                pc->setVelocity({0, 0, 0});
                pc->setGravityScale({0, 0, 0});
                pc->setVertices({
                    {-.5f, -.5f, 3.f},
                    {.5f,  -.5f, 3.f},
                    {.5f,  1.5f, 3.f},
                    {-.5f, 1.5f, 3.f},
                    {.5f,  -.5f, -3.f},
                    {-.5f, -.5f, -3.f},
                    {-.5f, 1.5f, -3.f},
                    {.5f,  1.5f, -3.f}
                });
                pc->setMass(99999999999);
                pc->setStatic(true);
            }
        }
        for (int i = 0; i < 9; i++) {
            auto fenceObj = std::make_shared<SpatialPawn>();
            fenceObj->setPosition({24.7 - 6.2 * i, 1, 557 - 1104 * k});
            fenceObj->setRotation(glm::rotate(glm::identity<glm::quat>(), glm::radians(90.0f), glm::vec3(0, 1, 0)));
            fenceObj->createComponent<RenderComponent>(gateModel[0]);
            sp->addPawnToRoot(fenceObj);
            auto pc = fenceObj->createComponent<PhysicsComponent>();
            pc->setVelocity({0, 0, 0});
            pc->setGravityScale({0, 0, 0});
            pc->setVertices({
                {-.5f, -.5f, 3.f},
                {.5f,  -.5f, 3.f},
                {.5f,  1.5f, 3.f},
                {-.5f, 1.5f, 3.f},
                {.5f,  -.5f, -3.f},
                {-.5f, -.5f, -3.f},
                {-.5f, 1.5f, -3.f},
                {.5f,  1.5f, -3.f}
            });
            pc->setMass(99999999999);
            pc->setStatic(true);
        }
    }

    auto overpassObj = std::make_shared<SpatialPawn>();
    overpassObj->setPosition({2, 5, 495});
    overpassObj->createComponent<RenderComponent>(overpassModel[0]);
    sp->addPawnToRoot(overpassObj);

    auto overpassObj2 = std::make_shared<SpatialPawn>();
    overpassObj2->setPosition({2, 5, -485});
    overpassObj2->createComponent<RenderComponent>(overpassModel[0]);
    sp->addPawnToRoot(overpassObj2);

    auto pl = std::make_shared<SpatialPawn>();
    pl->setPosition({2, 0, 505});
    pl->createComponent<RenderComponent>(playerModel[0]);
    pl->setRotation(glm::quat (glm::vec3(0, glm::radians(90.f), 0)));
    pl->setScale({3, 3, 3});
    sp->addPawnToRoot(pl);

    auto mirrorObj = std::make_shared<SpatialPawn>();
    mirrorObj->setPosition({10, -0.4, 505});
    mirrorObj->setRotation(glm::quat (glm::vec3(glm::radians(7.f), 0, 0)));
    mirrorObj->createComponent<RenderComponent>(mirrorModel[0]);
    mirrorObj->createComponent<RenderComponent>(mirrorModel[1]);
    sp->addPawnToRoot(mirrorObj);
    {
        auto wallObj = std::make_shared<SpatialPawn>();
        wallObj->setPosition({10, 0, 505.1});
        wallObj->setRotation(glm::quat (glm::vec3(glm::radians(7.f), 0, 0)));
        wallObj->setScale({0.9, 0.9, 0.9});
        auto object = wallObj->createComponent<RenderComponent>(mirrorModel[1]);


        glm::mat4 portal = glm::translate(glm::identity<glm::mat4>(), glm::vec3{0, 0, 1011});
        portal = glm::rotate(portal, glm::radians(180.0f), glm::vec3(1, 0, 0));
        portal = glm::scale(portal, glm::vec3 (1, -1, 1));

        object->getRtxRenderObject()->setPortal(portal);

        auto pc = wallObj->createComponent<PhysicsComponent>();
        pc->setVelocity({0, 0, 0});
        pc->setGravityScale({0, 0, 0});
        pc->setVertices({
            {-.5f, .5f, 0.2f},
            {.5f,  .5f, 0.2f},
            {.5f,  4.5f, 0.2f},
            {-.5f, 4.5f, 0.2f},
            {.5f,  .5f, -0.1f},
            {-.5f, .5f, -0.1f},
            {-.5f, 4.5f, -0.1f},
            {.5f,  4.5f, -0.1f}
        });
        pc->setMass(99999999999);
        pc->setStatic(true);

        sp->addPawnToRoot(wallObj);
    }

    auto flagObj = std::make_shared<SpatialPawn>();
    flagObj->setPosition({2, -10, -515});
    flagObj->setScale({5, 5, 5});
    flagObj->setRotation((glm::vec3(0, glm::radians(90.f), 0)));
    //flagObj->createComponent<RenderComponent>("assets/models/Flag.obj");
    flagObj->createComponent<RenderComponent>(flagModel[1]);
    flagObj->createComponent<RenderComponent>(flagModel[2]);
    flagObj->createComponent<RenderComponent>(flagModel[3]);
    sp->addPawnToRoot(flagObj);

    auto speakerObj = std::make_shared<SpatialPawn>();
    speakerObj->setPosition({-5, -0.15, 502});
    speakerObj->createComponent<RenderComponent>(speakersModel[0]);
    speakerObj->createComponent<RenderComponent>(speakersModel[1]);
//    speakerObj->createComponent<RenderComponent>("assets/models/Speakers.obj", 2);
//    speakerObj->createComponent<RenderComponent>("assets/models/Speakers.obj", 3);
    speakerObj->createComponent<SoundComponent>("assets/sounds/portal_radio_loop.ogg");
    sp->addPawnToRoot(speakerObj);

    //wall
    for (int i = 0; i < 2; i++) {
        auto wallObj = std::make_shared<SpatialPawn>();
        wallObj->setPosition({-300 + i * 604, 1.5, 495 + i * 0.0001});
        wallObj->createComponent<RenderComponent>(fenceModel[0]);
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
    for (int k = 0; k < 2; k++) {
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 2; i++) {
                auto wallObj = std::make_shared<SpatialPawn>();
                wallObj->setPosition({-304 + i * 612, 4.5 + 3 * j, 495 - k * 979.59});
                auto object = wallObj->createComponent<RenderComponent>(fenceModel1[0]);

                glm::mat4 portal = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, -980 + k * 1960));
                //portal = glm::rotate(portal, glm::radians(90.0f), glm::vec3(0, 1, 0));
                object->getRtxRenderObject()->setPortal(portal);

                sp->addPawnToRoot(wallObj);
            }
            {
                auto wallObj = std::make_shared<SpatialPawn>();
                wallObj->setPosition({2, 8 + 3 * j, 495 - k * 979.59});
                auto object = wallObj->createComponent<RenderComponent>(fenceModel1[0]);
                glm::mat4 portal = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, -980 + k * 1960));
                //portal = glm::rotate(portal, glm::radians(90.0f), glm::vec3(0, 1, 0));
                object->getRtxRenderObject()->setPortal(portal);
                sp->addPawnToRoot(wallObj);
            }
        }
    }

    //wall other side
    for (int i = 0; i < 2; i++) {
        auto wallObj = std::make_shared<SpatialPawn>();
        wallObj->setPosition({-300 + i * 604, 1.5, -485 + i * 0.0001});
        wallObj->createComponent<RenderComponent>(fenceModel[0]);
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

	//floor
	{
		auto pawn = std::make_shared<SpatialPawn>();
		pawn->createComponent<RenderComponent>(models[1]);
		sp->addPawnToRoot(pawn);
	}

    auto playerObj = std::make_shared<SpatialPawn>();
    playerObj->setPosition({-10, 4.2, 510});
    auto pc = playerObj->createComponent<PhysicsComponent>();
    auto mc = playerObj->createComponent<MovementComponent>();
    mc->setMaxSpeed(15);
    mc->setAcceleration(70);
    pc->setVelocity({0, 0, 0});
    pc->setGravityScale({0, 1, 0});
    pc->setVertices({
        {-1.f, -4.f, 1.f},
        {1.f, -4.f, 1.f},
        {1.f, .5f, 1.f},
        {-1.f, .5f, 1.f},
        {1.f, -4.f, -1.f},
        {-1.f, -4.f, -1.f},
        {-1.f, .5f, -1.f},
        {1.f, .5f, -1.f}
    });
    pc->setMass(12);
    pc->getMaterial().coefficient_of_restitution = -0.2f;
    pc->getMaterial().coefficient_of_friction = 3.f;
    sp->addPawnToRoot(playerObj);

    dispacher->registerListener(mc);

	auto camera_pawn = static_pointer_cast<SpatialPawn>(sp->getTree().findByName("Main Camera"));
	camera_pawn->setPosition({-10, 4, 505});
    //camera_pawn->setRotation(glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0)));
    static_pointer_cast<Camera>(camera_pawn->getComponents()[0])->setSpeed(0);
    playerObj->addChild(camera_pawn);

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
    }
    bool winTrigger = false;
    bool done = false;

	//window.getInputDispatcher().registerListener(std::make_shared<DebugInputListener>());
	while(!window.shouldClose()) {
		window.poll();

        glm::vec3 distance{playerObj->getPosition().x - flagObj->getPosition().x, 0, playerObj->getPosition().z - flagObj->getPosition().z};

        if (glm::length(distance) <= 0.5 && winTrigger == false)
        {
            winTrigger = true;
            auto sc = flagObj->createComponent<SoundComponent>("assets/sounds/win.ogg");
            sc->setLooping(false);
            printf("victory!");
        }

        camera_pawn->setPosition(playerObj->getPosition());
        float angle = atan2(camera_pawn->getForwardVector().z, camera_pawn->getForwardVector().x);
        pl->setPosition(playerObj->getPosition() + glm::rotate(glm::quat(glm::vec3(0, -angle, 0)),glm::vec3 (-0.65, -4, 0)));
        pl->setRotation(glm::quat (glm::vec3(0, -angle + glm::radians(90.f), 0)));

        mc->setDirection(static_pointer_cast<Camera>(camera_pawn->getComponents()[0])->getCamFacing());
        //roofObj->setPosition(roofObj->getPosition() + glm::vec3 {0.001, 0.001, 0.001});
		//physics update before rendering
		manager.updateCycle();
		std::shared_ptr<Board> current_board = manager.getCurrentBoard().lock();

		// draw render system overlay
		context->draw(system.getImmediateRenderer());

        if (!done) {
            done = true;
        }

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

        if (winTrigger)
        {
            RenderSystem& system = *RenderSystem::system;

            // create root panel
            auto context = std::make_shared<WidgetContext>();
            auto panel = theme.newRoot(system.width() / 2 - 100, system.height() / 2 - 50);

            auto settings = theme.newPanel();
            settings->flow = Flow::TOP_TO_BOTTOM;
            settings->gap = Unit::px(10);
            settings->width = Unit::px(250);
            settings->padding = Unit::px(10);
            settings->horizontal = HorizontalAlignment::LEFT;

            auto text = theme.newText("Victory!");
            text->size = 100;

            panel->addWidget(settings);
            settings->addWidget(text);

            //settings->horizontal = HorizontalAlignment::CENTER;

            {
                auto control = theme.newButton();
                control->width = Unit::px(100);
                control->horizontal = HorizontalAlignment::CENTER;

                auto label = theme.newText("Restart");

                control->addWidget(label);

                control->onClick ([&winTrigger, &playerObj, &context, &window]() -> void {
                    winTrigger = false;
                    playerObj->setPosition({-10, 4, 505});
                    window.getInputDispatcher().removeListener(context);
                    context.reset();

                });

                settings->addWidget(control);
            }

            context->setRoot(panel);
            context->draw(system.getImmediateRenderer());
            window.getInputDispatcher().registerListener(context, 1);
        }

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
