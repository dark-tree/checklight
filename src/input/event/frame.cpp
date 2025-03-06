
#include "frame.hpp"

/*
 * FrameEvent
 */

InputEvent::Type FrameEvent::getType() const {
	return FrameEvent::type;
}

void FrameEvent::capture() const {
	this->capture_flag = true;
}

void FrameEvent::close() const {
	close_flag = true;
}

void FrameEvent::cursor(CursorIcon::Icon icon) const {
	this->icon = icon;
}
