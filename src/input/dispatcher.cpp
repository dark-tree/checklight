
#include "dispatcher.hpp"

/*
 * InputDispatcher
 */

void InputDispatcher::registerListener(const std::shared_ptr<InputListener>& listener) {
	listeners.push_front(listener);
}

void InputDispatcher::removeListener(const std::shared_ptr<InputListener>& listener) {
	listeners.remove(listener);
}

InputResult InputDispatcher::onEvent(const InputEvent& event) {
	for (auto it = listeners.begin(); it != listeners.end(); it ++) {
		InputResult result = (*it)->onEvent(event);

		if (result != InputResult::PASS) {
			return result;
		}
	}

	return InputResult::PASS;
}
