#pragma once

#include "external.hpp"

/**
 * Represents cursor shape
 */
struct CursorIcon {

	enum Icon : int {
		DEFAULT = GLFW_ARROW_CURSOR,
		TEXT = GLFW_IBEAM_CURSOR,
		CROSSHAIR = GLFW_CROSSHAIR_CURSOR,
		POINTER = GLFW_POINTING_HAND_CURSOR,
		RESIZE = GLFW_RESIZE_ALL_CURSOR,
		HORIZONTAL = GLFW_HRESIZE_CURSOR,
		VERTICAL = GLFW_VRESIZE_CURSOR,
		BLOCKED = GLFW_NOT_ALLOWED_CURSOR,
	};

	static const char* toString(Icon icon);

};

