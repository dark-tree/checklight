#include "unit.hpp"

#include <gui/widget.hpp>
#include <shared/logger.hpp>

bool Unit::isResolvable() const {
	return (metric == Metric::PIXELS) || (metric == Metric::VIEWPORT_WIDTH) || (metric == Metric::VIEWPORT_HEIGHT);
}

int Unit::pixels(const StyleContext& context) const {
	if (metric == Metric::PIXELS) return value;
	if (metric == Metric::VIEWPORT_WIDTH) return value * context.width;
	if (metric == Metric::VIEWPORT_HEIGHT) return value * context.height;

	FAULT("Can't convert non-absolute unit to pixels!");
}
