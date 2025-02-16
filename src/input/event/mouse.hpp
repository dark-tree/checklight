#pragma once

#include "input/event.hpp"

/**
 * Mouse cursor movement input event
 */
class MouseEvent : public InputEvent {

	private:

		friend class Window;
		mutable bool capture_flag = false;

	public:

		static constexpr Type type = InputEvent::MOUSE;

		Type getType() const override;

	public:

		MouseEvent(double x, double y);

		/**
		 * Hide the mouse and capture it within the window,
		 * after the event is handled the mouse's state will be updated to match the state of the value set in the event
		 * By default the mouse will be un-capture, to capture effectively capture() must be called on each event received.
		 */
		void capture() const;

};
