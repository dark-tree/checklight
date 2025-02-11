#pragma once

#include "input/event.hpp"

class ScrollEvent : public InputEvent {

	public:

		static constexpr Type type = InputEvent::SCROLL;

		Type getType() const override;

	public:

		double horizontal;
		double vertical;

		ScrollEvent(double horizontal, double vertical, double x, double y);

};
