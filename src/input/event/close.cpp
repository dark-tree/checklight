
#include "close.hpp"

/*
 * CloseEvent
 */

void CloseEvent::abort() const {
	this->abort_flag = true;
}