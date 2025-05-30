#pragma once

#include "coded.hpp"

/**
 * Generic keyboard event, don't use for text input
 * @see UnicodeEvent
 */
class KeyboardEvent : public CodedEvent {

	public:

		int keycode;
		int scancode;

		KeyboardEvent(int keycode, int scancode, int action, int mods, double x, double y);

		/// Check if the given GLFW_KEY_* was pressed
		bool wasPressed(int keycode) const;

		/// Check is the given GLFW_KEY_* was released
		bool wasReleased(int keycode) const;

		/// Check if the given GLFW_KEY_* was typed
		bool wasTyped(int keycode) const;

};
