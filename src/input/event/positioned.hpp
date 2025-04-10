#pragma once

#include "input/event.hpp"

/**
 * Base class shared by all events that contain mouse position information
 */
class PositionedEvent : public InputEvent {

	public:

		double x;
		double y;

		PositionedEvent(double x, double y);

		/// Get mouse position as a glm::vec2 vector
		glm::vec2 getMouse() const;

};
