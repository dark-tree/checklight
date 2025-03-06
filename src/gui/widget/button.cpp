
#include "button.hpp"

#include <input/event/button.hpp>
#include <input/event/frame.hpp>
#include <input/event/mouse.hpp>
#include <render/immediate.hpp>

ButtonWidget::ButtonWidget(const std::string &label, const std::function<void()>& callback)
: enabled(true), hovered(false), pressed(false), label(label), callback(callback) {}

void ButtonWidget::draw(ImmediateRenderer& immediate) {

	if (hovered) {
		immediate.setColor(255, 80, 80);
	} else {
		immediate.setColor(255, 0, 0);
	}

	immediate.setRectRadius(10);
	immediate.drawRect2D(x, y, w, h);

	immediate.setFont("assets/font/OpenSans-Variable.ttf");
	immediate.setColor(0, 0, 0);
	immediate.setTextBox(w, h);
	immediate.drawText2D(x, y, label);
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

	if (auto* event = any.as<MouseEvent>()) {
		hovered = event->isWithinBox(x, y, w, h);
		return false;
	}

	if (auto* event = any.as<ButtonEvent>()) {
		return false;
	}

	return false;
}