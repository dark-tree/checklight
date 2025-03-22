
#include "keyboard.hpp"
#include "shared/logger.hpp"

/*
 * KeyboardEvent
 */

KeyboardEvent::KeyboardEvent(int keycode, int scancode, int action, int mods, double x, double y)
: CodedEvent(action, mods, x, y), keycode(keycode), scancode(scancode) {}

bool KeyboardEvent::wasPressed(int keycode) const {
	if (keycode < 0 || keycode > GLFW_KEY_LAST) {
		FAULT("Invalid keyboard key constant!");
	}

	return isPressEvent() && (this->keycode == keycode);
}

bool KeyboardEvent::wasReleased(int keycode) const {
	if (keycode < 0 || keycode > GLFW_KEY_LAST) {
		FAULT("Invalid keyboard key constant!");
	}

	return isReleaseEvent() && (this->keycode == keycode);
}

bool KeyboardEvent::wasTyped(int keycode) const {
	if (keycode < 0 || keycode > GLFW_KEY_LAST) {
		FAULT("Invalid keyboard key constant!");
	}

	return isTypedEvent() && (this->keycode == keycode);
}
