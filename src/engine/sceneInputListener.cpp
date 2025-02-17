#include "sceneInputListener.hpp"
#include "input/input.hpp"


SceneInputListener::SceneInputListener() : InputListener() {
	for (int i = 0; i < GLFW_KEY_LAST; i++) {
		sceneCurrentlyPressed[i] = false;
		toConsume[i] = false;
	}
	LMBPressed = false;
}

InputResult SceneInputListener::onEvent(const InputEvent& any) {
	if (const auto* event = any.as<KeyboardEvent>()) {
		if (event->action == GLFW_PRESS) {
			sceneCurrentlyPressed[event->keycode] = true;
			if (toConsume[event->keycode] == true)
				return InputResult::CONSUME;
		}
		if (event->action == GLFW_RELEASE) {
			sceneCurrentlyPressed[event->keycode] = false;
			if (toConsume[event->keycode] == true)
				return InputResult::CONSUME;
		}
	}
	if (const auto* event = any.as<ButtonEvent>()) {
		if (event->button == GLFW_MOUSE_BUTTON_LEFT) {
			if (event->action == GLFW_RELEASE) {
				LMBPressed = false;
			}
			else if (event->action == GLFW_PRESS) {
				LMBPressed = true;
			}
		}
	}

	if (const auto* event = any.as<MouseEvent>()) {
		mousePos = { event->x, event->y };
	}

	// ignore all other events
	// in our setup all of those will be routed to DebugInputListener
	return InputResult::PASS;
}
