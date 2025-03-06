
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

	// auto button = std::make_shared<ButtonWidget>("Hello", [] () {
	// 	printf("DEBUG: Pressed button!\n");
	// });

	auto button = std::make_shared<SliderWidget>([] () {

	});

	button->setBounds(600, 600, 100, 50);

	window.getInputDispatcher().registerListener(std::dynamic_pointer_cast<InputListener>(button));
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


		button->draw(system.getImmediateRenderer());

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
