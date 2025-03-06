
#include "widget.hpp"
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

InputResult Widget::onEvent(const InputEvent& any) {
	if (handle(any)) {
		return InputResult::BLOCK;
	}

	return InputResult::PASS;
}
