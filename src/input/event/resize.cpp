
#include "resize.hpp"

/*
 * ResizeEvent
 */

InputEvent::Type ResizeEvent::getType() const {
	return ResizeEvent::type;
}

ResizeEvent::ResizeEvent(int width, int height, double x, double y)
: InputEvent(x, y), width(width), height(height) {}
