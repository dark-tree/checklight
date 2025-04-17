#include "ease.hpp"

float ease::ofInOut(float time) {
	if (time < 0.5f) {
		// ease-in (accelerating)
		return 2 * time * time;
	}

	// ease-out (decelerating)
	return -1 + (4 - 2 * time) * time;
}

float ease::ofLinear(float time) {
	return time;
}