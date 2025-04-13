#pragma once

#include "external.hpp"
#include "shared/logger.hpp"
#include "unit.hpp"
#include "channel.hpp"

struct BoxUnit {

	Unit top, bottom, left, right;

	BoxUnit()
	: BoxUnit(Unit::zero()) {}

	BoxUnit(Unit unit)
	: BoxUnit(unit, unit) {}

	BoxUnit(Unit vertical, Unit horizontal)
	: BoxUnit(vertical, vertical, horizontal, horizontal) {}

	BoxUnit(Unit top, Unit bottom, Unit left, Unit right)
	: top(top), bottom(bottom), left(left), right(right) {
		if (!top.isResolvable()) FAULT("Top unit value must be resolvable!");
		if (!bottom.isResolvable()) FAULT("Bottom unit value must be resolvable!");
		if (!left.isResolvable()) FAULT("Left unit value must be resolvable!");
		if (!right.isResolvable()) FAULT("Right unit value must be resolvable!");
	}

	int along(const StyleContext& styling, Channel channel) {
		if (channel == Channel::WIDTH) {
			return left.pixels(styling) + right.pixels(styling);
		}

		return top.pixels(styling) + bottom.pixels(styling);
	}

};

constexpr bool operator == (const BoxUnit& lhs, const BoxUnit& rhs) {
	return lhs.top == rhs.top && lhs.bottom == rhs.bottom && lhs.left == rhs.left && lhs.right == rhs.right;
}

constexpr bool operator != (const BoxUnit& lhs, const BoxUnit& rhs) {
	return lhs.top != rhs.top || lhs.bottom != rhs.bottom || lhs.left != rhs.left || lhs.right != rhs.right;
}