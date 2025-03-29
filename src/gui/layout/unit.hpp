#pragma once

#include "external.hpp"
#include "shared/box.hpp"

enum struct Metric : uint64_t {
	FIT,
	GROW,
	PX
};

class Unit {

	public:

		double value;
		Metric metric;

		constexpr Unit()
		: value(0), metric(Metric::PX) {}

		constexpr Unit(double value, Metric metric)
		: value(value), metric(metric) {}

		bool isAbsolute() const;
		int toPixels() const;

	public:

		static constexpr Unit zero() {
			return {0, Metric::PX};
		}

		static constexpr Unit fit() {
			return {0, Metric::FIT};
		}

		static constexpr Unit grow(int fraction = 1) {
			return {static_cast<double>(fraction), Metric::GROW};
		}

		static constexpr Unit px(double value) {
			return {value, Metric::PX};
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

/*
 * Assertions
 */

static_assert(std::is_same<decltype(1_px), Unit>::value);
static_assert(std::is_same<decltype(1.0_px), Unit>::value);
