#pragma once

#include "external.hpp"

struct Sprite {
	float u1, v1, u2, v2;

	Sprite() = default;
	Sprite(float u1, float v1, float u2, float v2);

	/// Shrink sprite by margin pixels from each side
	Sprite shrink(float margin);

};