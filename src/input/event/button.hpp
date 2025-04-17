#pragma once

#include "coded.hpp"

/**
 * Mouse button input event
 */
class ButtonEvent : public CodedEvent {

	public:

		int button;

		ButtonEvent(int button, int action, int mods, double x, double y);

		/// Check if the given GLFW_MOUSE_BUTTON_* was pressed
		bool wasPressed(int button) const;

		/// Check if the given GLFW_MOUSE_BUTTON_* was released
		bool wasReleased(int button) const;

};
