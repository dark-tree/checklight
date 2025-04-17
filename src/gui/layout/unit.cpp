#include "unit.hpp"

#include <gui/widget.hpp>
#include <shared/logger.hpp>
#include <render/window.hpp>

bool Unit::isResolvable() const {
	return (metric == Metric::PIXELS) || (metric == Metric::VIEWPORT_WIDTH) || (metric == Metric::VIEWPORT_HEIGHT);
}

int Unit::pixels() const {
	if (metric == Metric::PIXELS) return value;
	if (metric == Metric::VIEWPORT_WIDTH) return value * Viewport::getCurrent().width;
	if (metric == Metric::VIEWPORT_HEIGHT) return value * Viewport::getCurrent().height;

	FAULT("Can't convert non-absolute unit to pixels!");
}
