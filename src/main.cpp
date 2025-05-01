
#include <gui/context.hpp>
#include <gui/theme/dark.hpp>
#include <gui/theme/light.hpp>
#include <gui/widget/panel.hpp>
#include <gui/widget/root.hpp>
#include <gui/widget/text.hpp>
#include <shared/args.hpp>
#include <shared/logger.hpp>

#include "render/render.hpp"
#include "input/input.hpp"
#include "engine/engine.hpp"
#include "gui/gui.hpp"
#include "engine/entity/component/matrixAnimation.hpp"

static void entry(Args& args) {

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game!");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);
	Models::init();

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();

	AutoTheme theme {};
	auto context = std::make_shared<WidgetContext>();
	auto panel = theme.newRoot(20, 20);
	auto text = theme.newText("Język lechicki z grupy zachodniosłowiańskiej");

	{

		auto left = theme.newPanel();
		left->flow = Flow::TOP_TO_BOTTOM;
		left->gap = Unit::px(10);

		auto sub2 = theme.newButton("Apply");
		auto slider = theme.newSlider();

		sub2->onClick([=] () {
			out::info("Clicked left button!");
			text->setText("Slider value: " + std::to_string(int(100 * slider->getValue())));
		});

		left->addWidget(sub2);
		left->addWidget(slider);

		slider->onChange([] (float value) {
			out::info("Slider value changed to: %f", value);
		});

		auto select = theme.newSelect({"EEVEE", "Cycles", "Checklight"});
		auto field = theme.newTextField();

		left->addWidget(select);
		left->addWidget(field);

		panel->addWidget(left);
	}

	{
		auto sub = theme.newImage("assets/image/corners.png");
		panel->addWidget(sub);

		sub->width = Unit::px(100);
		sub->height = Unit::px(100);
		sub->min_width = Unit::px(100);
		sub->min_height = Unit::px(100);
		sub->background = {100, 100, 100};
		sub->radius = Unit::px(10);
		sub->border = Unit::px(2);
		sub->border_color = {0, 0, 0};
		sub->padding = Unit::px(10);
		sub->margin = Unit::px(10);
	}

	{
		auto sub = theme.newPanel();
		panel->addWidget(sub);

		sub->background = {250, 100, 100};
		sub->addWidget(text);
	}

	panel->flow = Flow::LEFT_TO_RIGHT;
	panel->padding = Unit::px(10);
	panel->gap = Unit::px(20);
	panel->vertical = VerticalAlignment::CENTER;
	panel->horizontal = HorizontalAlignment::CENTER;
	// panel->width = Unit::px(400);

	context->setRoot(panel);

	//window.getInputDispatcher().registerListener(std::make_shared<DebugInputListener>());
	window.getInputDispatcher().registerListener(std::dynamic_pointer_cast<InputListener>(context));
	auto models = system.importObj("assets/models/checklight.obj");
	auto cube = system.importObj("assets/models/cube.obj");

	BoardManager manager(window);

	std::shared_ptr<Pawn> pawn = std::make_shared<Pawn>();

	auto pawnThatRendersTheCube = std::make_shared<SpatialPawn>();
	auto pawnThatRendersTheSphere = std::make_shared<SpatialPawn>();

	pawnThatRendersTheCube->setPosition({-2,1,-2});

	pawnThatRendersTheSphere->setPosition({2,1,2});

	std::shared_ptr<Component> renderSphere = std::make_shared<RenderComponent>(Models::SPHERE);
	std::shared_ptr<Component> renderCube = std::make_shared<RenderComponent>(Models::CUBE);

	std::shared_ptr<Component> rotateCube = std::make_shared<MatrixAnimation>(MatrixAnimation::ROTATE);
	std::shared_ptr<Component> moveSphere = std::make_shared<MatrixAnimation>(MatrixAnimation::TRANSLATE);


	pawnThatRendersTheSphere->addComponent(renderSphere);
	pawnThatRendersTheSphere->addComponent(moveSphere);

	pawnThatRendersTheCube->addComponent(renderCube);
	pawnThatRendersTheCube->addComponent(rotateCube);


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

	while (!window.shouldClose()) {
		window.poll();

		//physics update before rendering
		manager.updateCycle();
		std::shared_ptr<Board> current_board = manager.getCurrentBoard().lock();

		context->draw(system.getImmediateRenderer());

		//drawUserInterface(system.getImmediateRenderer(), system.width(), system.height());

		// update uniforms
		// do this once at the beginning of frame rendering
		system.setProjectionMatrix(65.0f, 0.01f, 1000.0f);
		system.setViewMatrix(current_board->getCamPos(), current_board->getCamForward());

		// update lights
		point_light->vector = glm::vec3(3.0, 2.0, 18.0 * sin(glfwGetTime() / 8));
		point_light->color = glm::vec3(sin(glfwGetTime() / 2) * 0.5 + 0.5, sin(glfwGetTime() / 3 + 2) * 0.5 + 0.5, sin(glfwGetTime() / 5 + 4) * 0.5 + 0.5);
		system.getLightManager().flush();

		// render the scene
		system.draw();
	}

	system.wait();
	Models::terminate();//temporary

	for (auto& object : objects) {
		object.reset();
	}

	models.clear();

	for (auto& model : cube) {
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
