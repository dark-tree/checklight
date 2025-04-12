#pragma once

#include "external.hpp"

struct ElementState {

	enum Lifecycle {
		LAYOUT = 0,
		DRAW = 1,
	};

	enum Interaction {
		DISABLED = 1,
		DEFAULT = 2,
		HOVER = 3,
		PRESSED = 4,
	};

	Lifecycle lifecycle;     ///< Check if the timeing of the state request
	Interaction interaction; ///< Check user interaction state
	bool focused;            ///< Is the element selected with a navigator?

	/*
	 * Helpers
	 */

	static ElementState ofLayout() {
		return {LAYOUT, DEFAULT, false};
	}

	static ElementState ofDraw(Interaction interaction, bool focused) {
		return {DRAW, interaction, focused};
	}

	static ElementState ofDraw() {
		return ofDraw(DEFAULT, false);
	}

};