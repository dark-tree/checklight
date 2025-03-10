
#include "box.hpp"

/*
 * Box2D
 */

Box2D::Box2D(int x, int y, int w, int h) {
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

Box2D Box2D::split_top_bottom(int height) {
	int yn = this->y;

	this->y += height;
	this->h -= height;

	return {x, yn, w, height};
}

Box2D Box2D::split_left_right(int width) {
	int xn = this->x;

	this->x += width;
	this->w -= width;

	return {xn, y, width, h};
}

Box2D Box2D::expand(int top, int bottom, int left, int right) const {
	int dx = left + right;
	int dy = top + bottom;

	return Box2D {x - left, y - bottom, w + dx, h + dy};
}

Box2D Box2D::reserve(int min_width, int min_height) const {
	Box2D copy = *this;

	copy.w = std::max(min_width, copy.w);
	copy.h = std::max(min_height, copy.h);

	return copy;
}

bool Box2D::empty() const {
	return (w == 0) || (h == 0);
}

Box2D Box2D::allocate(std::list<Box2D>& boxes, int w, int h) {

	for (auto it = boxes.begin(); it != boxes.end(); ++it) {

		Box2D& box = *it;

		// remove degenerate boxes
		if (box.empty()) {
			it = boxes.erase(it);
			continue;
		}

		// our box needs to fit first
		if (box.w < w || box.h < h) {
			continue;
		}

		// a happy little accident!
		if (box.w == w && box.h == h) {
			Box2D match = box;
			boxes.erase(it);
			return match;
		}

		//  width
		if (box.w == w) {
			return box.split_top_bottom(h);
		}

		// height
		if (box.h == h) {
			return box.split_left_right(w);
		}

		Box2D left = box.split_left_right(w);
		Box2D bottom = left.split_top_bottom(h);

		boxes.push_front(left);
		return bottom;

	}

	return {0, 0, 0, 0};

}