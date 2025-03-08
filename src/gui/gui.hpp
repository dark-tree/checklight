#pragma once

#include "widget/button.hpp"
#include "widget/slider.hpp"
#include "widget/select.hpp"
#include "widget/input.hpp"

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

	immediate.setFont("assets/font/OpenSans-Variable.ttf");
	// immediate.setFontSize(100);
	// immediate.drawText2D(300, 200, "Cześć Świecie!");
	//
	// immediate.setColor(200, 0, 0);
	// immediate.setSprite(OFF);
	// immediate.setLineWidth(1);
	// immediate.drawLine2D(300, 200, 800, 200);
	//
	// immediate.setColor(255, 255, 255);
	immediate.setFontSize(30);
	// immediate.drawText2D(300, 220, "Checklight - Game engine based on the Vulkan API");

	immediate.setSprite("assets/image/corners.png");
	immediate.setLineWidth(0.1);
	immediate.drawLine3D(0, 0, 0, sin(glfwGetTime() / 3) * 50, 10, cos(glfwGetTime() / 3) * 50);
	immediate.drawRect3D(sin(glfwGetTime() / 3) * 50, 10, cos(glfwGetTime() / 3) * 50, 2, 2);

	immediate.setTextAlignment(VerticalAlignment::CENTER);
	immediate.setTextAlignment(HorizontalAlignment::CENTER);
	immediate.setRectRadius(5);
	immediate.setSprite(OFF);
	immediate.setColor(0, 0, 0);
	immediate.drawRect2D(100, 100, 300, 200);

	immediate.setColor(255, 255, 255);
	immediate.setTextBox(300, 200);
	immediate.drawString2D(100, 100, "Checkmate!");

	// immediate.setTextBox(OFF);
	// immediate.setSprite(OFF);
	// immediate.setRectRadius(10, 20, 40, 80);
	// // immediate.drawRect2D(100, 100, 300, 200);
	// immediate.drawCircle2D(400, 400, 80);

}
