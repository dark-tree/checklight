
#include "select.hpp"

#include "input/input.hpp"
#include "render/immediate.hpp"
#include "gui/context.hpp"

SelectWidget::SelectWidget(const std::function<void()>& callback)
: InputWidget(), unrolled(false), callback(callback) {

	for (int i = 0; i < 5; i ++) {
		options.emplace_back("Option: " + std::to_string(i));
	}

}

void SelectWidget::draw(ImmediateRenderer& immediate) {

	if (isFocused()) {
		immediate.setRectRadius(5);
		immediate.setColor(255, 255, 0);
		immediate.drawRect2D(x - 8, y - 8, w + 16, h + 16);
	} else {
		unrolled = false;
	}

	if (enabled) {
		if (hovered) {
			if (pressed) {
				immediate.setColor(255, 100, 100);
			} else {
				immediate.setColor(255, 80, 80);
			}
		} else {
			immediate.setColor(255, 0, 0);
		}
	} else {
		immediate.setColor(70, 70, 70);
	}

	// background
	immediate.setRectRadius(0);
	immediate.drawRect2D(x, y, w, h);

	immediate.setFont("assets/font/OpenSans-Variable.ttf");
	immediate.setColor(0, 0, 0);
	immediate.setTextBox(w, h);
	immediate.drawString2D(x, y, "Selected: " + std::to_string(value));

	if (!unrolled) {
		return;
	}

	// options
	for (int i = 0; i < options.size(); i ++) {
		int oy = y + h * (i + 1);

		// background
		if (option == i) {
			if (pressed) {
				immediate.setColor(255, 100, 100);
			} else {
				immediate.setColor(255, 80, 80);
			}
		} else {
			immediate.setColor(255, 0, 0);
		}

		immediate.drawRect2D(x, oy, w, h);

		// content
		immediate.setFont("assets/font/OpenSans-Variable.ttf");
		immediate.setColor(0, 0, 0);
		immediate.setTextBox(w, h);
		immediate.drawString2D(x, oy, options.at(i).label);
	}

}

bool SelectWidget::event(WidgetContext& context, const InputEvent& any) {

	if (!enabled) {
		return false;
	}

	if (auto* event = any.as<FrameEvent>()) {
		if (hovered || unrolled) {
			event->cursor(CursorIcon::POINTER);
			return true;
		}

		return false;
	}

	if (auto* keyboard = any.as<KeyboardEvent>()) {
		if (keyboard->isReleaseEvent()) {
			if (keyboard->keycode == GLFW_KEY_ESCAPE && unrolled) {
				unrolled = false;
				option = value;
				pressed = false;
				return true;
			}

			if (keyboard->keycode == GLFW_KEY_ENTER) {
				if (unrolled) {
					value = option;
				} else {
					option = value;

					if (option == OPTION_NONE) {
						option = 0;
					}
				}

				unrolled = !unrolled;
				pressed = unrolled;
				return true;
			}
		}

		if (keyboard->isTypedEvent()) {
			if (keyboard->keycode == GLFW_KEY_DOWN && unrolled) {
				option = std::min((int) options.size() - 1, option + 1);
				return true;
			}

			if (keyboard->keycode == GLFW_KEY_UP && unrolled) {
				option = std::max(0, option - 1);
				return true;
			}
		}

		return false;
	}

	if (auto* positioned = any.as<PositionedEvent>()) {
		hovered = positioned->isWithinBox(x, y, w, h);

		if (unrolled) {
			for (int i = 0; i < options.size(); i ++) {
				int oy = y + h * (i + 1);
				bool entry = positioned->isWithinBox(x, oy, w, h);

				if (entry) {
					option = i;
				}
			}
		}

		if (auto* button = positioned->as<ButtonEvent>()) {

			if (isFocused() || hovered) {

				// this is a bit of a hack as I couldn't get the
				// logic below to comply with my desires, the multitude
				// of flags and conditions here make this a real pain to deal with
				// so we just force 'false' artificially here to cancel selection
				if (!unrolled && !hovered && isFocused()) {
					return false;
				}

				if (button->isPressEvent()) {
					pressed = true;
					setFocus(context);
					return true;
				}

				if (button->isReleaseEvent()) {
					pressed = false;
					unrolled = hovered && !unrolled;
					value = option;
					return true;
				}
			}
		}

	}

	return false;
}