
#include "scroll.hpp"

/*
 * ScrollEvent
 */

InputEvent::Type ScrollEvent::getType() const {
	return ScrollEvent::type;
}

ScrollEvent::ScrollEvent(double horizontal, double vertical, double x, double y)
: InputEvent(x, y), horizontal(horizontal), vertical(vertical) {}