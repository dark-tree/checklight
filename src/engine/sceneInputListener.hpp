#pragma once
#include "input/listener.hpp"
#include "external.hpp"

class SceneInputListener : public InputListener {
public:
	bool sceneCurrentlyPressed[GLFW_KEY_LAST];
	bool toConsume[GLFW_KEY_LAST];
	bool LMBPressed;
	glm::vec2 mousePos;

	SceneInputListener();

	InputResult onEvent(const InputEvent& any) override;
};