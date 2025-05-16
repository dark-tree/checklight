
#include "positioned.hpp"
#include "shared/box.hpp"

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

bool PositionedEvent::isWithinBox(const Box2D& box) const {
	return isWithinBox(box.x, box.y, box.w, box.h);
}
