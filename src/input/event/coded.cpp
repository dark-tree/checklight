
#include "coded.hpp"

/*
 * CodedEvent
 */

CodedEvent::CodedEvent(int action, int mods, double x, double y)
: PositionedEvent(x, y), action(action), mods(mods) {}

bool CodedEvent::isPressEvent() const {
	return action == GLFW_PRESS;
}

bool CodedEvent::isReleaseEvent() const {
	return action == GLFW_RELEASE;
}

bool CodedEvent::isTypedEvent() const {
	return isPressEvent() || (action == GLFW_REPEAT);
}

bool CodedEvent::isShiftPressed() const {
	return mods & GLFW_MOD_SHIFT;
}

bool CodedEvent::isCtrlPressed() const {
	return mods & GLFW_MOD_CONTROL;
}

bool CodedEvent::isAltPressed() const {
	return mods & GLFW_MOD_ALT;
}