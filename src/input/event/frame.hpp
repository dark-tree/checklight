#pragma once

#include "input/event.hpp"
#include "input/icon.hpp"

/**
 * Event emitted each frame, used to gain permission to control the window
 */
class FrameEvent : public InputEvent {

	private:

		friend class Window;
		mutable bool capture_flag : 1 = false;
		mutable bool close_flag : 1 = false;
		mutable CursorIcon::Icon icon = CursorIcon::DEFAULT;

	public:

		FrameEvent() = default;

		/**
		 * Hide the mouse and capture it within the window,
		 * after the event is handled the mouse's state will be updated to match the state of the value set in the event
		 * By default the mouse will be un-captured, to capture effectively capture() must be called on each event received.
		 */
		void capture() const;

		/**
		 * Break the render loop and close the program window,
		 * if you aborted the CloseEvent this is how you can later close the window yourself.
		 * By default, this flag is set to NOT close the window.
		 */
		 void close() const;

		/**
		 * Change the mouse icon to the one specified,
		 * after the event is handled the mouse's icon will be updated.
		 * By default, if no action is taken, the mouse will revert to the default arrow.
		 */
		void cursor(CursorIcon::Icon icon) const;

};
