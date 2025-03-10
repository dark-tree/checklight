#include "unit.hpp"

int Unit::resolve(int parent) const {
	if (metric == Metric::PX) return static_cast<int>(value);
	if (metric == Metric::PR) return static_cast<int>(value * parent);

	return 0;
}