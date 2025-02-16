#pragma once

#include "input/event.hpp"

/**
 * Base class shared by keyboard and mouse button input events
 */
class CodedEvent : public InputEvent {

	public:

		int action;
		int mods;

		CodedEvent(int action, int mods, double x, double y);

		/// Check if the key/button was pressed
		bool isPressEvent() const;

		/// Check if the key/button was released
		bool isReleaseEvent() const;

		/// Check if the SHIFT key was held
		bool isShiftPressed() const;

		/// Check if the CTRL key was held
		bool isCtrlPressed() const;

		/// Check if the ALT key was held
		bool isAltPressed() const;

};
