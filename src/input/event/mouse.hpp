#pragma once

#include "positioned.hpp"

/**
 * Mouse cursor movement input event
 */
class MouseEvent : public PositionedEvent {

	public:

		MouseEvent(double x, double y);

};
