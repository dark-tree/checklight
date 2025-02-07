#pragma once

#include "input/event.hpp"

class MouseEvent : public InputEvent {

	public:

		static constexpr Type type = InputEvent::MOUSE;

		Type getType() const override;

	public:

		MouseEvent(double x, double y);

};
