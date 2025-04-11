#pragma once

#include "external.hpp"

enum struct ElementState : int {
	// only shows up during initial layout calculation
	LAYOUT = 0,

	// used during rendering based on user actions
	DISABLED = 1,
	DEFAULT = 2,
	HOVER = 3,
	PRESSED = 4,
};