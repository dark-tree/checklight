
#include <gui/context.hpp>
#include <gui/widget/panel.hpp>
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

	auto context = std::make_shared<WidgetContext>();
	auto panel = std::make_shared<PanelWidget>();

	{
		auto sub = std::make_shared<TextWidget>("Lorem ipsum dolor sit amet");
		panel->addWidget(sub);
	}

	{
		auto sub = std::make_shared<PanelWidget>();
		panel->addWidget(sub);

		sub->width = Unit::px(100);
		sub->height = Unit::px(100);
		sub->min_width = Unit::px(100);
		sub->min_height = Unit::px(100);
		sub->r = 100;
		sub->g = 100;
		sub->b = 250;
		sub->padding = Unit::px(10);

		sub->margin = Unit::px(10);
	}

	{
		auto sub = std::make_shared<PanelWidget>();
		panel->addWidget(sub);

		sub->width = Unit::fit();
		sub->height = Unit::fit();
		sub->r = 250;
		sub->g = 100;
		sub->b = 100;

		auto text = std::make_shared<TextWidget>("Język lechicki z grupy zachodniosłowiańskiej");
		sub->addWidget(text);
	}

	panel->width = Unit::fit();
	panel->height = Unit::fit();
	panel->flow = Flow::LEFT_TO_RIGHT;
	panel->padding = Unit::px(10);
	panel->gap = Unit::px(20);
	panel->vertical = VerticalAlignment::CENTER;
	panel->horizontal = HorizontalAlignment::CENTER;

	panel->rebuild(10, 10);

	// auto slider = std::make_shared<SliderWidget>([] () noexcept {
	//
	// });
	// slider->setBounds({600, 300, 100, 50});
	//
	// auto button = std::make_shared<ButtonWidget>("Hello", [] () noexcept {
	// 	out::debug("Pressed button!");
	// });
	// button->setBounds({600, 400, 100, 50});
	//
	// auto input = std::make_shared<FieldWidget>([] () noexcept {
	//
	// });
	// input->setBounds({600, 500, 100, 50});
	//
	// auto select = std::make_shared<SelectWidget>([] () noexcept {
	//
	// });
	// select->setBounds({600, 600, 100, 50});

	context->setRoot(panel);
	// panel->addWidget(slider);
	// panel->addWidget(button);
	// panel->addWidget(input);
	// panel->addWidget(select);

	//window.getInputDispatcher().registerListener(std::make_shared<DebugInputListener>());
	window.getInputDispatcher().registerListener(std::dynamic_pointer_cast<InputListener>(context));
	auto models = system.importObj("assets/models/checklight.obj");

	BoardManager manager;

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

	//--------------------------------
	{
		BoardManager manager;
		std::shared_ptr<Board> board = manager.getCurrentBoard().lock();

		std::shared_ptr<Pawn> r1 = std::make_shared<Pawn>();
		std::shared_ptr<Pawn> r2 = std::make_shared<Pawn>();
		std::shared_ptr<Pawn> r3 = std::make_shared<Pawn>();

		board->addPawnToRoot(r1);
		board->addPawnToRoot(r2);
		board->addPawnToRoot(r3);

		std::shared_ptr<Pawn> r4 = std::make_shared<Pawn>();
		std::shared_ptr<Pawn> r5 = std::make_shared<Pawn>();
		std::shared_ptr<Pawn> r6 = std::make_shared<Pawn>();

		r4->addChild(r5);
		board->addPawnToRoot(r4);

		r5->addChild(r6);
	}
	//------------------------------------------


	std::vector<std::shared_ptr<RenderObject>> objects;

	for (auto& model : models) {
		auto object = system.createRenderObject();
		object->setMatrix(glm::identity<glm::mat4x3>());
		object->setModel(model);
		objects.push_back(object);
	}

	while (!window.shouldClose()) {
		window.poll();

		//physics update before rendering
		manager.updateCycle();
		std::shared_ptr<Board> current_board = manager.getCurrentBoard().lock();

		context->draw(system.getImmediateRenderer());

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
