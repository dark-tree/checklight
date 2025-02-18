#pragma once

#include "positioned.hpp"

/**
 * Mouse cursor movement input event
 */
class MouseEvent : public PositionedEvent {

	public:

		static constexpr Type type = InputEvent::MOUSE;

		Type getType() const override;

	public:

		MouseEvent(double x, double y);

};
