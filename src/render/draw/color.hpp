#pragma once

#include <external.hpp>

class Color {

	public:

		uint8_t r = 255;
		uint8_t g = 255;
		uint8_t b = 255;
		uint8_t a = 255;

	public:

		Color() = default;
		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

		Color multiplyAlpha(uint8_t a) const;
};

constexpr bool operator == (const Color& lhs, const Color& rhs) {
	return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

constexpr bool operator != (const Color& lhs, const Color& rhs) {
	return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b || lhs.a != rhs.a;
}
