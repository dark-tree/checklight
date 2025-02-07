
#include "debug.hpp"
#include "input.hpp"

/*
 * DebugInputListener
 */

std::string DebugInputListener::modifiersToString(int modifiers) const {
	std::vector<std::string> array;

	if (modifiers & GLFW_MOD_SHIFT) array.emplace_back("GLFW_MOD_SHIFT");
	if (modifiers & GLFW_MOD_CONTROL) array.emplace_back("GLFW_MOD_CONTROL");
	if (modifiers & GLFW_MOD_ALT) array.emplace_back("GLFW_MOD_ALT");
	if (modifiers & GLFW_MOD_SUPER) array.emplace_back("GLFW_MOD_SUPER");
	if (modifiers & GLFW_MOD_CAPS_LOCK) array.emplace_back("GLFW_MOD_CAPS_LOCK");
	if (modifiers & GLFW_MOD_NUM_LOCK) array.emplace_back("GLFW_MOD_NUM_LOCK");

	if (array.empty()) {
		return "[]";
	}

	return "[" + std::accumulate(
		std::next(array.begin()),
		array.end(),
		array.front(),
		[] (const std::string& a, const std::string& b) {
			return a + ", " + b;
		}
	) + "]";
}

std::string DebugInputListener::actionToString(int action) const {
	if (action == GLFW_RELEASE) return "GLFW_RELEASE";
	if (action == GLFW_PRESS) return "GLFW_PRESS";
	if (action == GLFW_REPEAT) return "GLFW_REPEAT";

	return std::to_string(action);
}

InputResult DebugInputListener::onEvent(const InputEvent& any) {
	if (const auto* event = any.as<ButtonEvent>()) {
		std::string action = actionToString(event->action);
		std::string modifier = modifiersToString(event->mods);

		printf("DEBUG: ButtonEvent {button=%d, action=%s, modifier=%s, x=%d, y=%d}\n", event->button, action.c_str(), modifier.c_str(), (int) event->x, (int) event->y);
	}

	if (const auto* event = any.as<KeyboardEvent>()) {
		std::string action = actionToString(event->action);
		std::string modifier = modifiersToString(event->mods);

		printf("DEBUG: KeyboardEvent {keycode=%d, scancode=%d, action=%s, modifier=%s, x=%d, y=%d}\n", event->keycode, event->scancode, action.c_str(), modifier.c_str(), (int) event->x, (int) event->y);
	}

	if (const auto* event = any.as<ScrollEvent>()) {
		printf("DEBUG: ScrollEvent {vertical=%d, horizontal=%d, x=%d, y=%d}\n", (int) event->vertical, (int) event->horizontal, (int) event->x, (int) event->y);
	}

	if (const auto* event = any.as<UnicodeEvent>()) {
		printf("DEBUG: UnicodeEvent {unicode=%u, x=%d, y=%d}\n", event->unicode, (int) event->x, (int) event->y);
	}

	// don't interfere with normal event processing, "ignore" all events
	return InputResult::PASS;
}