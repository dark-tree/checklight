
#include "button.hpp"
#include "shared/logger.hpp"

/*
 * ButtonEvent
 */

ButtonEvent::ButtonEvent(int button, int action, int mods, double x, double y)
: CodedEvent(action, mods, x, y), button(button) {}

bool ButtonEvent::wasPressed(int button) const {
	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
		FAULT("Invalid mouse button constant!");
	}

	return isPressEvent() && (this->button == button);
}

bool ButtonEvent::wasReleased(int button) const {
	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
		FAULT("Invalid mouse button constant!");
	}

	return isReleaseEvent() && (this->button == button);
}