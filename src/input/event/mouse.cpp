
#include "mouse.hpp"

/*
 * MouseEvent
 */

InputEvent::Type MouseEvent::getType() const {
	return MouseEvent::type;
}

MouseEvent::MouseEvent(double x, double y)
: PositionedEvent(x, y) {}
