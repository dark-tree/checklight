
#include "dispatcher.hpp"

/*
 * InputDispatcher
 */

void InputDispatcher::registerListener(const std::shared_ptr<InputListener>& listener, int priority) {
	listeners.insert(priority, listener);
}

void InputDispatcher::removeListener(const std::shared_ptr<InputListener>& listener) {
	listeners.remove(listener);
}

InputResult InputDispatcher::onEvent(const InputEvent& event) {

	InputResult feedback = InputResult::PASS;

	for (auto& listener : listeners) {
		InputResult result = listener->onEvent(event);

		// event ignored, continue processing
		if (result == InputResult::PASS) {
			continue;
		}

		// stop further processing in this context
		if (result == InputResult::YIELD) {
			return InputResult::PASS;
		}

		// stop further processing in calling context
		if (result == InputResult::ACCEPT) {
			feedback = InputResult::BLOCK;
			continue;
		}

		// stop further processing in this and calling context
		if (result == InputResult::BLOCK) {
			return InputResult::BLOCK;
		}

	}

	return feedback;
}

