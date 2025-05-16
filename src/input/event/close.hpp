#pragma once

#include "input/event.hpp"

/**
 * Window close input event
 */
class CloseEvent : public InputEvent {

	private:

		friend class Window;
		mutable bool abort_flag = false;

	public:

		CloseEvent() = default;

		/**
		 * Abort the automatic window closing,
		 * for example to show a confirmation message or save game state.
		 * By default, unless abort() is closed the window will close and application terminate
		 * after returning from this event.
		 */
		void abort() const;

};
