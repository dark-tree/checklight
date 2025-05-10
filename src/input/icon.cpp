
#include "icon.hpp"

const char* CursorIcon::toString(Icon icon) {

	switch (icon) {
		case DEFAULT: return "Default";
		case TEXT: return "Text";
		case CROSSHAIR: return "Crosshair";
		case POINTER: return "Pointer";
		case RESIZE: return "Resize";
		case HORIZONTAL: return "Horizontal";
		case VERTICAL: return "Vertical";
		case BLOCKED: return "Blocked";
		default: return "Undefined";
	}

}