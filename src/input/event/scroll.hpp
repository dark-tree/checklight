#pragma once

#include "positioned.hpp"

/**
 * Mouse wheel scroll input event
 */
class ScrollEvent : public PositionedEvent {

	public:

		double horizontal;
		double vertical;

		ScrollEvent(double horizontal, double vertical, double x, double y);

		/// Get mouse scroll amount as a glm::vec2 vector
		glm::vec2 getScroll() const;

};
