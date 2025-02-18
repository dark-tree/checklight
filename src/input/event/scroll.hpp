#pragma once

#include "positioned.hpp"

/**
 * Mouse wheel scroll input event
 */
class ScrollEvent : public PositionedEvent {

	public:

		static constexpr Type type = InputEvent::SCROLL;

		Type getType() const override;

	public:

		double horizontal;
		double vertical;

		ScrollEvent(double horizontal, double vertical, double x, double y);

};
