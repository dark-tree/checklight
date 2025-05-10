#pragma once

#include "external.hpp"
#include "shared/logger.hpp"
#include "unit.hpp"

struct RadiusUnit {

	Unit top_left, top_right, bottom_left, bottom_right;

	RadiusUnit()
	: RadiusUnit(Unit::zero()) {}

	RadiusUnit(Unit unit)
	: RadiusUnit(unit, unit) {}

	RadiusUnit(Unit top, Unit bottom)
	: RadiusUnit(top, top, bottom, bottom) {}

	RadiusUnit(Unit top_left, Unit top_right, Unit bottom_left, Unit bottom_right)
	: top_left(top_left), top_right(top_right), bottom_left(bottom_left), bottom_right(bottom_right) {
		if (!top_left.isResolvable()) FAULT("Top left unit value must be resolvable!");
		if (!top_right.isResolvable()) FAULT("Top right unit value must be resolvable!");
		if (!bottom_left.isResolvable()) FAULT("Bottom left unit value must be resolvable!");
		if (!bottom_right.isResolvable()) FAULT("Bottom right unit value must be resolvable!");
	}

};

constexpr bool operator == (const RadiusUnit& lhs, const RadiusUnit& rhs) {
	return lhs.top_left == rhs.top_left && lhs.top_right == rhs.top_right && lhs.bottom_left == rhs.bottom_left && lhs.bottom_right == rhs.bottom_right;
}

constexpr bool operator != (const RadiusUnit& lhs, const RadiusUnit& rhs) {
	return lhs.top_left != rhs.top_left || lhs.top_right != rhs.top_right || lhs.bottom_left != rhs.bottom_left || lhs.bottom_right != rhs.bottom_right;
}