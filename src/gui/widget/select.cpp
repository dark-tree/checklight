
#include "select.hpp"

#include <input/debug.hpp>
#include <input/event/button.hpp>
#include <input/event/frame.hpp>
#include <input/event/keyboard.hpp>
#include <input/event/mouse.hpp>
#include <render/immediate.hpp>

SelectWidget::SelectWidget(const std::function<void()>& callback)
: Widget(), unrolled(false), callback(callback) {

	for (int i = 0; i < 5; i ++) {
		options.emplace_back("Option: " + std::to_string(i));
	}

}

void SelectWidget::draw(ImmediateRenderer& immediate) {

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

bool SelectWidget::handle(const InputEvent& any) {

	DebugInputListener debugger {};
	debugger.onEvent(any);

	if (!enabled) {
		return false;
	}

	if (auto* event = any.as<FrameEvent>()) {
		if (hovered || (option != OPTION_NONE)) {
			event->cursor(CursorIcon::POINTER);
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

			if (button->isPressEvent()) {
				pressed = true;
			}

			if (button->isReleaseEvent()) {
				pressed = false;
				unrolled = hovered && !unrolled;
				value = option;
			}
		}

		if (auto* keyboard = positioned->as<KeyboardEvent>()) {

			if (keyboard->isReleaseEvent()) {
				if (keyboard->keycode == GLFW_KEY_ESCAPE && unrolled) {
					unrolled = false;
					option = value;
					pressed = false;
				}

				if (keyboard->keycode == GLFW_KEY_ENTER) {
					if (unrolled) {
						value = option;
					} else {
						option = value;
					}

					unrolled = !unrolled;
					pressed = unrolled;
				}

				if (keyboard->keycode == GLFW_KEY_DOWN && unrolled) {
					option = std::min((int) options.size() - 1, option + 1);
				}

				if (keyboard->keycode == GLFW_KEY_UP && unrolled) {
					option = std::max(0, option - 1);
				}
			}

		}

		return false;
	}


	return false;
}