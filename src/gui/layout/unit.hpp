#pragma once

#include "external.hpp"
#include "shared/box.hpp"

struct StyleContext;

enum struct Metric : uint64_t {
	FIT,
	GROW,
	PIXELS,
	VIEWPORT_WIDTH,
	VIEWPORT_HEIGHT,
};

struct Unit {

	public:

		double value;
		Metric metric;

		constexpr Unit()
		: value(0), metric(Metric::PIXELS) {}

		constexpr Unit(double value, Metric metric)
		: value(value), metric(metric) {}

		bool isAbsolute() const;
		int pixels(const StyleContext& styling) const;

	public:

		// hook for StyleProperty
		static constexpr auto unwrapper = &Unit::pixels;

		static constexpr Unit zero() {
			return {0, Metric::PIXELS};
		}

		static constexpr Unit fit() {
			return {0, Metric::FIT};
		}

		static constexpr Unit grow(int fraction = 1) {
			return {static_cast<double>(fraction), Metric::GROW};
		}

		static constexpr Unit px(double value) {
			return {value, Metric::PIXELS};
		}

		static constexpr Unit vh(double value) {
			return {value, Metric::VIEWPORT_HEIGHT};
		}

		static constexpr Unit vw(double value) {
			return {value, Metric::VIEWPORT_WIDTH};
		}

};


/*
 * Unit Operators
 */

constexpr Unit operator - (const Unit& unit) {
	return {- unit.value, unit.metric};
}

