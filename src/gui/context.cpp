
#include "context.hpp"
#include "input/input.hpp"
#include "widget.hpp"

/*
 * WidgetContext
 */

std::vector<std::shared_ptr<InputWidget>> WidgetContext::getSelectableWidgets() const {
	std::vector<std::shared_ptr<InputWidget>> widgets;
	root->appendSelectable(widgets);
	return widgets;
}

void WidgetContext::setSelected(const std::shared_ptr<InputWidget>& widget) {
	if (selected) {
		selected->setSelected(false);
	}

	selected = widget;
	widget->setSelected(true);
}

void WidgetContext::setRoot(const std::shared_ptr<Widget>& widget) {
	root = widget;
}

void WidgetContext::draw(ImmediateRenderer& immediate) {
	root->draw(immediate);
}

InputResult WidgetContext::onEvent(const InputEvent& any) {
	if (const auto* keyboard = any.as<KeyboardEvent>()) {
		if (keyboard->wasReleased(GLFW_KEY_TAB)) {
			auto selectable = getSelectableWidgets();
			int i = 0;

			// find the currently selected element
			if (selected) {
				auto it = std::find(selectable.begin(), selectable.end(), selected);

				if (it != selectable.end()) {
					i = std::distance(selectable.begin(), it) + 1;

					// clamp to array size
					if (i > selectable.size()) {
						i = 0;
					}
				}
			}

			setSelected(selectable[i]);
			return InputResult::BLOCK;
		}
	}

	return root->event(*this, any) ? InputResult::BLOCK : InputResult::PASS;
}


