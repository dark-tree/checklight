#pragma once

#include "widget/button.hpp"
#include "widget/slider.hpp"
#include "widget/select.hpp"
#include "widget/field.hpp"

inline void drawUserInterface(ImmediateRenderer& immediate, float width, float height) {


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

	immediate.setMatrix2D(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, (sin(5 + glfwGetTime() * 8) + 1) / 16, 0)));
	immediate.setSprite("assets/image/mug12.png");
	immediate.drawCircle2D(350, 50, 40);
	immediate.setMatrix2D(glm::identity<glm::mat4>());

	immediate.setSprite("assets/image/vulkan-1.png");
	immediate.drawRect2D(0, height - 126, 310, 126);

	int w = 100 + (sin(glfwGetTime()) + 1) * 100;
	int h = 100;

	immediate.setSprite(OFF);
	immediate.setColor(100, 100, 200);
	immediate.setRectRadius(5);
	immediate.drawRect2D(500, 500, w, h);

	immediate.setTextAlignment(VerticalAlignment::CENTER);
	immediate.setTextAlignment(HorizontalAlignment::CENTER);
	immediate.setColor(0, 0, 0);
	immediate.setFont("assets/font/OpenSans-Variable.ttf");
	immediate.setFontSize(30);
	immediate.setTextBox(w, h);
	immediate.drawString2D(500, 500, "Hello Lorem Ipsum Muggum World Ambassador Delight Silksong");



	immediate.setSprite("assets/image/corners.png");
	immediate.setLineWidth(0.1);
	immediate.drawLine3D(0, 0, 0, sin(glfwGetTime() / 3) * 50, 10, cos(glfwGetTime() / 3) * 50);
	immediate.drawRect3D(sin(glfwGetTime() / 3) * 50, 10, cos(glfwGetTime() / 3) * 50, 2, 2);
	immediate.setSprite(OFF);

	immediate.setColor(0, 0, 0);
	glm::vec2 pos = RenderSystem::system->getWindow().getInputContext().getCursorPosition();
	immediate.drawCircle2D(pos.x, pos.y, 5);

}
