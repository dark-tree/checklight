#pragma once

#include "external.hpp"
#include "radius.hpp"
#include "sided.hpp"
#include "unit.hpp"
#include "shared/trait.hpp"

template <Numerical T>
inline T interpolate(const T& start, const T& end, float delta) {
	return static_cast<T>(end * delta + start * (1.0f - delta));
}

inline Unit interpolate(const Unit& start, const Unit& end, float delta) {
	if (start.metric == end.metric) {
		return Unit {interpolate(start.value, end.value, delta), start.metric};
	}

	if (start.isResolvable() && end.isResolvable()) {
		// TODO resolve and interpolate on pixels
	}

	// nothing we can do
	return (delta < 0.5f) ? start : end;
}

inline BoxUnit interpolate(const BoxUnit& start, const BoxUnit& end, float delta) {
	return BoxUnit {
		interpolate(start.top, end.top, delta),
		interpolate(start.bottom, end.bottom, delta),
		interpolate(start.left, end.left, delta),
		interpolate(start.right, end.right, delta),
	};
}

inline RadiusUnit interpolate(const RadiusUnit& start, const RadiusUnit& end, float delta) {
	return RadiusUnit {
		interpolate(start.top_left, end.top_left, delta),
		interpolate(start.top_right, end.top_right, delta),
		interpolate(start.bottom_left, end.bottom_left, delta),
		interpolate(start.bottom_right, end.bottom_right, delta),
	};
}

inline Color interpolate(const Color& start, const Color& end, float delta) {
	return Color { // this is not the correct way to interpolate color but it will do for now
		interpolate(start.r, end.r, delta),
		interpolate(start.g, end.g, delta),
		interpolate(start.b, end.b, delta),
		interpolate(start.a, end.a, delta),
	};
}