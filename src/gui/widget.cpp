
#include "widget.hpp"
#include "input/event/button.hpp"
#include "render/immediate.hpp"

/*
 * Widget
 */

void Widget::setBounds(int x, int y, int w, int h) {
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

Widget::~Widget() {
	// noop base virtual destructor
}

bool Widget::handle(const InputEvent& any) {

	// this check will need to be repeated by the child class
	if (!enabled) {
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
			}
		}
	}

	return false;
}

InputResult Widget::onEvent(const InputEvent& any) {
	if (handle(any)) {
		return InputResult::BLOCK;
	}

	return InputResult::PASS;
}
