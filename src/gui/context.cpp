
#include "context.hpp"
#include "input/input.hpp"
#include "widget.hpp"

/*
 * WidgetContext
 */

void WidgetContext::setSelected(NULLABLE const std::shared_ptr<InputWidget>& widget) {
	navigator.setSelected(widget);
}

void WidgetContext::setRoot(const std::shared_ptr<Widget>& widget) {
	root = widget;

	// obtain navigable elements
	Navigator navigator;
	root->scan(navigator);
	this->navigator = navigator;
}

void WidgetContext::draw(ImmediateRenderer& immediate) {
	root->draw(immediate, ElementState::DEFAULT);
}

InputResult WidgetContext::onEvent(const InputEvent& any) {
	if (const auto* keyboard = any.as<KeyboardEvent>()) {
		if (keyboard->wasReleased(GLFW_KEY_TAB)) {
			navigator.next();
			return InputResult::BLOCK;
		}
	}

	const bool used = root->event(*this, any);

	if (!used) {
		if (any.as<ButtonEvent>()) {
			setSelected(nullptr);
		}

		return InputResult::PASS;
	}

	return InputResult::BLOCK;
}


