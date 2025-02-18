
#include "close.hpp"

/*
 * CloseEvent
 */

InputEvent::Type CloseEvent::getType() const {
	return CloseEvent::type;
}

void CloseEvent::abort() const {
	this->abort_flag = true;
}