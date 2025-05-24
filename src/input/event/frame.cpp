
#include "frame.hpp"

/*
 * FrameEvent
 */

void FrameEvent::capture() const {
	this->capture_flag = true;
}

void FrameEvent::close() const {
	close_flag = true;
}

void FrameEvent::cursor(CursorIcon::Icon icon) const {
	this->icon = icon;
}
