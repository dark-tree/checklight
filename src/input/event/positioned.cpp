
#include "positioned.hpp"

/*
 * PositionedEvent
 */

PositionedEvent::PositionedEvent(double x, double y)
: x(x), y(y) {}

glm::vec2 PositionedEvent::getMouse() const {
	return {x, y};
}

bool PositionedEvent::isWithinBox(int x, int y, int w, int h) const {
	int dx = this->x - x;
	int dy = this->y - y;

	if (dx < 0 || dy < 0 || dx > w || dy > h) {
		return false;
	}

	return true;
}