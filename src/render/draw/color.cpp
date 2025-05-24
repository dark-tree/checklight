
#include "color.hpp"

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
: r(r), g(g), b(b), a(a) {}

Color Color::multiplyAlpha(uint8_t a) const {
	return {r, g, b, static_cast<uint8_t>((this->a / 255.0f) * (a / 255.0f) * 255)};
}