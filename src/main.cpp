
#include <gui/context.hpp>
#include <gui/widget/panel.hpp>

#include "render/render.hpp"
#include "input/input.hpp"
#include "engine/engine.hpp"
#include "gui/gui.hpp"

int main() {
	std::string path = std::filesystem::current_path().generic_string();
	printf("INFO: Current working directory: %s\n", path.c_str());

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game!");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();

	auto context = std::make_shared<WidgetContext>();
	auto panel = std::make_shared<PanelWidget>();
	auto sub = std::make_shared<PanelWidget>();

	sub->width = Unit::px(100);
	sub->height = Unit::px(100);
	sub->g = 50;
	sub->b = 200;
	sub->vertical = VerticalAlignment::CENTER;

	auto sub2 = std::make_shared<PanelWidget>();

	sub2->width = Unit::px(100);
	sub2->height = Unit::px(100);
	sub2->g = 200;
	sub2->b = 50;
	sub2->vertical = VerticalAlignment::CENTER;
	sub2->horizontal = HorizontalAlignment::CENTER;

	panel->addWidget(sub);
	panel->addWidget(sub2);
	panel->width = Unit::px(400);
	panel->height = Unit::px(400);

	panel->setBounds({10,10,0,0});


	// auto slider = std::make_shared<SliderWidget>([] () noexcept {
	//
	// });
	// slider->setBounds({600, 300, 100, 50});
	//
	// auto button = std::make_shared<ButtonWidget>("Hello", [] () noexcept {
	// 	printf("DEBUG: Pressed button!\n");
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

	BoardManager m(window);

	std::shared_ptr<Pawn> p = std::make_shared<Pawn>();
	p->setName("Test");

	m.getCurrentBoard().lock()->addPawnToRoot(p);

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
		m.updateCycle();
		std::shared_ptr<Board> current_board = m.getCurrentBoard().lock();

		context->draw(system.getImmediateRenderer());

		drawUserInterface(system.getImmediateRenderer(), system.width(), system.height());

		// update uniforms
		// do this once at the beginning of frame rendering
		system.setProjectionMatrix(40.0f, 0.001f, 10000.0f);
		system.setViewMatrix(current_board->getCamPos(), current_board->getCamForward());

		// render the scene
		system.draw();
	}

	system.wait();

	for (auto& object : objects) {
		object.reset();
	}

	for (auto& model : models) {
		system.closeModel(model);
		model.reset();
	}

	RenderSystem::system.reset();

}
