#pragma once

#include "input/event.hpp"

class MouseEvent : public InputEvent {

	private:

		friend class Window;
		mutable bool capture = false;

	public:

		static constexpr Type type = InputEvent::MOUSE;

		Type getType() const override;

	public:

		MouseEvent(double x, double y);

		/**
		 * Set to true to hide the mouse and capture it within the window,
		 * after the event is handled the mouse's state will be updated to match the state of the value set in the event
		 * By default this value is set to false, to capture it must be set on each event received.
		 */
		void setCapture(bool capture) const;

};
