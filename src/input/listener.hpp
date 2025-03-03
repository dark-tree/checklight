#pragma once

#include "event.hpp"
#include "result.hpp"

/**
 * Implement this interface on all classes that should accept input events
 * and register them to the input dispatcher
 */
class InputListener {

	public:

		/**
		 * Process or pass input events.
		 *
		 * @param event The event that was received and needs processing
		 * @return State of input processing
		 */
		virtual InputResult onEvent(const InputEvent& event) = 0;

};
