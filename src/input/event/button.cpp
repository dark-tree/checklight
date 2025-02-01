
#include "button.hpp"

/*
 * ButtonEvent
 */

InputEvent::Type ButtonEvent::getType() const {
	return ButtonEvent::type;
}

ButtonEvent::ButtonEvent(int button, int action, int mods, double x, double y)
: CodedEvent(action, mods, x, y), button(button) {}

bool ButtonEvent::wasPressed(int button) const {
	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
		throw std::runtime_error {"Invalid mouse button constant!"};
	}

	return isPressEvent() && (this->button == button);
}

bool ButtonEvent::wasReleased(int button) const {
	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
		throw std::runtime_error {"Invalid mouse button constant!"};
	}

	return isReleaseEvent() && (this->button == button);
}