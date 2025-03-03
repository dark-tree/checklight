
#include "resize.hpp"

/*
 * ResizeEvent
 */

InputEvent::Type ResizeEvent::getType() const {
	return ResizeEvent::type;
}

ResizeEvent::ResizeEvent(int width, int height)
: width(width), height(height) {}
