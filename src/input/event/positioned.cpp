
#include "positioned.hpp"

/*
 * PositionedEvent
 */

PositionedEvent::PositionedEvent(double x, double y)
: x(x), y(y) {}

glm::vec2 PositionedEvent::getMouse() const {
	return {x, y};
}