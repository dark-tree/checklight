
#include "render/render.hpp"

void drawUserInterface(ImmediateRenderer& immediate, float width, float height) {
	immediate.setSprite("assets/image/corners.png");

	immediate.setLayer(100);
	immediate.setLineWidth(4);
	immediate.setColor(255, 255, 255);
	immediate.drawLine2D(10, 10, 100, 500);

	immediate.setColor(50, 50, 100);
	immediate.setRectRadius(10, 20, 40, 80);
	immediate.drawRect2D(100, 100, 200, 150);

	immediate.setColor(200, 200, 200);
	immediate.setRectRadius(10);
	immediate.drawRect2D(300, 300, 400, 400);

	immediate.setColor(0, 0, 0);
	immediate.drawCircle2D(800, 100, 50);

	immediate.setLineWidth(20);
	immediate.setColor(255, 255, 255);
	immediate.drawEllipse2D(800, 100, 20, 40);
	immediate.drawBezier2D(800, 100, 900, 400, 500, 600, 800, 800);

	immediate.setLayer(0);
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

	immediate.setSprite("assets/image/watermark.png");
	immediate.setRectRadius(0);
	immediate.drawRect2D(width / 2 - 200, 0, 400, 80);

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

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();

	auto meshes = RenderSystem::importObj("assets/models/checklight.obj");

	auto models = system.createRenderModels(meshes);

	auto commander = system.createTransientCommander();
	system.rebuildBottomLevel(commander->getRecorder());
	commander->complete();

	std::vector<std::shared_ptr<RenderObject>> objects;

	for (auto& model : models) {
		auto object = system.createRenderObject();
		object->setMatrix(glm::identity<glm::mat4x3>());
		object->setModel(model);
		objects.push_back(object);
	}
	
	while (!window.shouldClose()) {
		window.poll();

		drawUserInterface(system.getImmediateRenderer(), system.width(), system.height());

		// update uniforms
		// do this once at the beginning of frame rendering
		system.setProjectionMatrix(40.0f, 0.1f, 1000.0f);
		system.setViewMatrix({18.0f, 1, 4.0f}, {-21.0f, 0.0f, 4.0f});

		// render the scene
		system.draw();
	}

	system.wait();

	for (auto& object : objects) {
		object.reset();
	}

	for (auto& model : models) {
		model.reset();
	}

	for (auto& mesh : meshes) {
		mesh.reset();
	}

	RenderSystem::system.reset();

}
