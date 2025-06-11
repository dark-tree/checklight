#pragma once

#include "listener.hpp"

/**
 * Helper listener for monitoring received input events
 */
class DebugInputListener : public InputListener {

	private:

		bool show_mouse;

		std::string modifiersToString(int modifiers) const;
		std::string actionToString(int action) const;

	public:

		/// By default mouse events are not shown, pass true to enable displaying them
		DebugInputListener(bool show_mouse = false);

		/// DebugInputListener will always PASS events
		InputResult onEvent(const InputEvent &event) override;

};
