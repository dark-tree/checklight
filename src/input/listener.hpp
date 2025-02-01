#pragma once

#include "event.hpp"

/**
 * InputResult informs the InputDispatcher
 * on the state of the InputEvent handling
 */
enum struct InputResult {
	BLOCK,   ///< Return if (for example) some UI elements blocks all events (like inventory blocking movement)
	CONSUME, ///< Return if the event was acted on (some action was taken), blocks further processing
	PASS,    ///< Return if the event was ignored and should continue listener traversal
};

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
