
#include "select.hpp"

#include <input/event/button.hpp>
#include <input/event/frame.hpp>
#include <input/event/mouse.hpp>
#include <render/immediate.hpp>

SelectWidget::SelectWidget(const std::function<void()>& callback)
: enabled(true), unrolled(false), pressed(false), callback(callback) {}

void SelectWidget::draw(ImmediateRenderer& immediate) {

	if (enabled) {
		if (hovered == OPTION_HEADER) {
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
	immediate.drawText2D(x, y, "Selected: " + std::to_string(value));

	if (!unrolled) {
		return;
	}

	// options
	for (int i = 0; i < 10; i ++) {
		int oy = y + h * (i + 1);

		// background
		if (hovered == i) {
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
		immediate.drawText2D(x, oy, "Option: " + std::to_string(i));
	}

}

bool SelectWidget::handle(const InputEvent& any) {

	if (!enabled) {
		return false;
	}

	if (auto* event = any.as<FrameEvent>()) {
		if (hovered != OPTION_NONE) {
			event->cursor(CursorIcon::POINTER);
		}
		return false;
	}

	if (auto* positioned = any.as<PositionedEvent>()) {
		const bool header = positioned->isWithinBox(x, y, w, h);
		hovered = header ? OPTION_HEADER : OPTION_NONE;

		if (unrolled) {
			for (int i = 0; i < 10; i ++) {
				int oy = y + h * (i + 1);
				bool entry = positioned->isWithinBox(x, oy, w, h);

				if (entry) {
					hovered = i;
				}
			}
		}

		if (auto* button = positioned->as<ButtonEvent>()) {

			if (button->isPressEvent()) {
				pressed = true;
			}

			if (button->isReleaseEvent()) {
				pressed = false;
				unrolled = header && !unrolled;
				value = hovered;
			}
		}

		return false;
	}

	return false;
}