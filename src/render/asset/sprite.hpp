#pragma once

#include "external.hpp"

struct Sprite {
	int u1, v1, u2, v2;

	Sprite() = default;
	Sprite(int u1, int v1, int u2, int v2);
};