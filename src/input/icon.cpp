
#include "icon.hpp"

const char* CursorIcon::toString(Icon icon) {

	switch (icon) {
		case CursorIcon::DEFAULT: return "Default";
		case CursorIcon::TEXT: return "Text";
		case CursorIcon::CROSSHAIR: return "Crosshair";
		case CursorIcon::POINTER: return "Pointer";
		case CursorIcon::RESIZE: return "Resize";
		case CursorIcon::HORIZONTAL: return "Horizontal";
		case CursorIcon::VERTICAL: return "Vertical";
		case CursorIcon::BLOCKED: return "Blocked";
		default: return "Undefined";
	}

}