#include "unit.hpp"

#include <shared/logger.hpp>

bool Unit::isAbsolute() const {
	return metric == Metric::PX;
}

int Unit::toPixels() const {
	if (metric == Metric::PX) return value;

	FAULT("Can't convert non-absolute unit to pixels!");
}
