
#include "close.hpp"

/*
 * CloseEvent
 */

InputEvent::Type CloseEvent::getType() const {
	return CloseEvent::type;
}

CloseEvent::CloseEvent(double x, double y)
: InputEvent(x, y) {}

void CloseEvent::abort() const {
	this->abort_flag = true;
}