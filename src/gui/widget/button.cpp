
#include "button.hpp"

#include <input/event/button.hpp>
#include <input/event/frame.hpp>
#include <input/event/mouse.hpp>
#include <render/immediate.hpp>

ButtonWidget::ButtonWidget(const std::string &label, const std::function<void()>& callback)
: enabled(true), hovered(false), pressed(false), label(label), callback(callback) {}

void ButtonWidget::draw(ImmediateRenderer& immediate) {

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

	immediate.setRectRadius(10);
	immediate.drawRect2D(x, y, w, h);

	immediate.setFont("assets/font/OpenSans-Variable.ttf");
	immediate.setColor(0, 0, 0);
	immediate.setTextBox(w, h);
	immediate.drawString2D(x, y, label);
}

bool ButtonWidget::handle(const InputEvent& any) {

	if (!enabled) {
		return false;
	}

	if (auto* event = any.as<FrameEvent>()) {
		if (hovered) {
			event->cursor(CursorIcon::POINTER);
		}
		return false;
	}

	if (auto* positioned = any.as<PositionedEvent>()) {
		hovered = positioned->isWithinBox(x, y, w, h);

		if (!hovered) {
			pressed = false;
			return false;
		}

		if (auto* button = positioned->as<ButtonEvent>()) {
			if (button->isPressEvent()) {
				pressed = true;
			}

			if (button->isReleaseEvent()) {
				pressed = false;
				callback();
			}
		}

		return false;
	}

	return false;
}