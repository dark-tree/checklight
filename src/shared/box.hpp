#pragma once

#include "external.hpp"

struct Box2D {

	int x, y, w, h;

	Box2D(int x, int y, int w, int h);

	/// Split this box into it's top and bottom parts
	Box2D split_top_bottom(int offset);

	/// Split this box into it's left and right parts
	Box2D split_left_right(int offset);

	/// Return a box with added space on each side
	Box2D expand(int top, int bottom, int left, int right) const;

	/// Expands the box if needed to be at least min_width by min_height
	Box2D reserve(int min_width, int min_height) const;

	/// Check if no pixels are bound by this box
	bool empty() const;

	/// Allocate area from pool
	static Box2D allocate(std::list<Box2D>& boxes, int w, int h);

};
