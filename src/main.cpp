#include "render/render.hpp"
#include "input/input.hpp"
#include "engine/engine.hpp"
#include "sound/sound.hpp"

static void drawUserInterface(ImmediateRenderer& immediate, float width, float height) {
	immediate.setSprite("assets/image/corners.png");

	immediate.setMatrix2D(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, (sin(1 + glfwGetTime() * 8) + 1) / 16, 0)));
	immediate.setSprite("assets/image/skay.png");
	immediate.setColor(255, 255, 255);
	immediate.drawCircle2D(50, 50, 40);

	immediate.setMatrix2D(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, (sin(2 + glfwGetTime() * 8) + 1) / 16, 0)));
	immediate.setSprite("assets/image/lucek.png");
	immediate.drawCircle2D(150, 50, 40);

	immediate.setMatrix2D(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, (sin(3 + glfwGetTime() * 8) + 1) / 16, 0)));
	immediate.setSprite("assets/image/wiesiu.png");
	immediate.drawCircle2D(250, 50, 40);

	immediate.setMatrix2D(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, (sin(4 + glfwGetTime() * 8) + 1) / 16, 0)));
	immediate.setSprite("assets/image/magistermaks.png");
	immediate.drawCircle2D(350, 50, 40);

	immediate.setMatrix2D(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, (sin(5 + glfwGetTime() * 8) + 1) / 16, 0)));
	immediate.setSprite("assets/image/mug12.png");
	immediate.drawCircle2D(450, 50, 40);
	immediate.setMatrix2D(glm::identity<glm::mat4>());

	immediate.setSprite("assets/image/vulkan-1.png");
	immediate.drawRect2D(0, height - 126, 310, 126);

	immediate.setFont("assets/font/OpenSans-Variable.ttf");
	immediate.setFontSize(100);
	immediate.drawText2D(300, 200, "Cześć Świecie!");

	immediate.setColor(200, 0, 0);
	immediate.setSprite(OFF);
	immediate.setLineWidth(1);
	immediate.drawLine2D(300, 200, 800, 200);

	immediate.setColor(255, 255, 255);
	immediate.setFontSize(25);
	immediate.drawText2D(300, 220, "Checklight - Game engine based on the Vulkan API");

	immediate.setSprite("assets/image/corners.png");
	immediate.setLineWidth(0.1);
	immediate.drawLine3D(0, 0, 0, sin(glfwGetTime() / 3) * 50, 10, cos(glfwGetTime() / 3) * 50);
	immediate.drawRect3D(sin(glfwGetTime() / 3) * 50, 10, cos(glfwGetTime() / 3) * 50, 2, 2);

}

int main() {

	std::string path = std::filesystem::current_path().generic_string();
	printf("INFO: Current working directory: %s\n", path.c_str());

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game!");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();

	//window.getInputDispatcher().registerListener( std::make_shared<DebugInputListener>());
	auto models = system.importObj("assets/models/checklight.obj");
	auto cube = system.importObj("assets/models/cube.obj");

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

	system.getLightManager().addLight({
		.type = Light::DIRECTIONAL,
		.color = glm::vec3(1.0, 1.0, 1.0),
		.direction = glm::vec3(0.0, 3.5, -1.0),
		.intensity = 1.5,
		.shadow = true
	});

	auto point_light = system.getLightManager().addLight({
		.type = Light::POINT,
		.position = glm::vec3(3.0, 2.0, 18.0),
		.color = glm::vec3(0.0, 0.0, 1.0),
		.intensity = 50.0,
		.shadow = true
	});

	SoundListener::setPosition(0, 0, 0);
	SoundManager& sm = SoundManager::getInstance();
	auto sso = std::make_shared<SoundSourceObject>();
	sm.addSource(sso);
	sm.createSoundClipAndAddToSourceObject("assets/sounds/testWAV.wav", sso);
	sso->setPosition(0, 0, 0);
	sso->setMaxDistance(20);
	sso->setReferenceDistance(1.0f);
	sso->setRolloffFactor(1.0f);
	sso->setMinGain(0.0f);
	SoundListener::setDistanceModel(AL_INVERSE_DISTANCE);

	while (!window.shouldClose()) {
		window.poll();

		//physics update before rendering
		m.updateCycle();
		std::shared_ptr<Board> current_board = m.getCurrentBoard().lock();

		drawUserInterface(system.getImmediateRenderer(), system.width(), system.height());

		// update uniforms
		// do this once at the beginning of frame rendering
		system.setProjectionMatrix(65.0f, 0.01f, 1000.0f);
		system.setViewMatrix(current_board->getCamPos(), current_board->getCamForward());

		// update lights
		point_light->position = glm::vec3(3.0, 2.0, 18.0 * sin(glfwGetTime() / 8));
		point_light->color = glm::vec3(sin(glfwGetTime() / 2) * 0.5 + 0.5, sin(glfwGetTime() / 3 + 2) * 0.5 + 0.5, sin(glfwGetTime() / 5 + 4) * 0.5 + 0.5);
		system.getLightManager().flush();
		
		// render the scene
		system.draw();
		sm.playSound(sso);
		SoundListener::setPosition(current_board->getCamPos().x, current_board->getCamPos().y, current_board->getCamPos().z);
	}

	system.wait();

	for (auto& object : objects) {
		object.reset();
	}

	for (auto& model : models) {
		system.closeModel(model);
		model.reset();
	}

	for (auto& model : cube) {
		system.closeModel(model);
		model.reset();
	}

	RenderSystem::system.reset();

}
