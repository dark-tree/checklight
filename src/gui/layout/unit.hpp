#pragma once

#include "external.hpp"
#include "shared/box.hpp"

enum struct Metric : uint64_t {
	NONE,
	PX, // Pixels
	PR, // Precentage
};

class Unit {

	public:

		double value;
		Metric metric;

		constexpr Unit()
		: value(0), metric(Metric::NONE) {}

		constexpr Unit(double value, Metric metric)
		: value(value), metric(metric) {}

		int resolve(int parent) const;

	public:

		static constexpr Unit px(double value) {
			return {value, Metric::PX};
		}

		static constexpr Unit pr(double value) {
			return {value, Metric::PR};
		}

};

/*
 * Unit Operators
 */

constexpr Unit operator - (const Unit& unit) {
	return {- unit.value, unit.metric};
}

/*
 * Unit Literals
 */

constexpr Unit operator ""_px(long double value) {
	return Unit::px(static_cast<double>(value));
}

constexpr Unit operator ""_px(unsigned long long int value) {
	return Unit::px(static_cast<double>(value));
}

constexpr Unit operator ""_pr(long double value) {
	return Unit::pr(static_cast<double>(value));
}

constexpr Unit operator ""_pr(unsigned long long int value) {
	return Unit::pr(static_cast<double>(value));
}

/*
 * Assertions
 */

static_assert(std::is_same<decltype(1_px), Unit>::value);
static_assert(std::is_same<decltype(1.0_px), Unit>::value);
static_assert(std::is_same<decltype(1_pr), Unit>::value);
static_assert(std::is_same<decltype(1.0_pr), Unit>::value);
