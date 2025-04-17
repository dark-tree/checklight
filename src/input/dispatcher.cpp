
#include "dispatcher.hpp"
#include "shared/logger.hpp"

/*
 * InputDispatcher
 */

void InputDispatcher::registerListener(const std::shared_ptr<InputListener>& listener, int priority) {
	if (!listener) {
		FAULT("Given input listener is null!");
	}

	listeners.insert(priority, listener);
}

void InputDispatcher::registerLowerListener(const std::shared_ptr<InputListener>& listener) {
	registerListener(listener, listeners.empty() ? 0 : (listeners.lowest() - 1));
}

void InputDispatcher::registerHigherListener(const std::shared_ptr<InputListener>& listener) {
	registerListener(listener, listeners.empty() ? 0 : (listeners.highest() + 1));
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

