
#include "widget.hpp"
#include "input/input.hpp"
#include "render/immediate.hpp"
#include "context.hpp"

/*
 * Widget
 */

void Widget::setBounds(int x, int y, int w, int h) {
	padded = {x, y, w, h};
	content = padded.expand(-8, -8, -8, -8);
}

Box2D Widget::getContentBox() const {
	return content;
}

Box2D Widget::getPaddingBox() const {
	return padded;
}

Box2D Widget::getMarginBox() const {
	return padded;
}

Widget::~Widget() {
	// no-op base virtual destructor
}

bool Widget::event(WidgetContext& context, const InputEvent& any) {
	return false;
}

void Widget::appendSelectable(std::vector<std::shared_ptr<InputWidget>>& selectable) {
	// no-op base virtual method
}

/*
 * InputWidget
 */

bool InputWidget::isFocused() const {
	return selected;
}

void InputWidget::setFocus(WidgetContext& context) {
	context.setSelected(std::dynamic_pointer_cast<InputWidget>(shared_from_this()));
}


void InputWidget::setSelected(bool selected) {
	this->selected = selected;
}

bool InputWidget::event(WidgetContext& context, const InputEvent& any) {

	// this check will need to be repeated by the child class
	if (!enabled) {
		return false;
	}

	if (auto* positioned = any.as<PositionedEvent>()) {
		hovered = positioned->isWithinBox(padded);

		if (!hovered && !positioned->as<KeyboardEvent>()) {
			pressed = false;
			return false;
		}

		if (auto* button = positioned->as<ButtonEvent>()) {
			if (button->isPressEvent()) {
				pressed = true;
				setFocus(context);
				return true;
			}

			if (button->isReleaseEvent()) {
				pressed = false;
				return true;
			}
		}
	}

	return false;
}

void InputWidget::appendSelectable(std::vector<std::shared_ptr<InputWidget>>& selectable) {
	selectable.push_back(std::dynamic_pointer_cast<InputWidget>(shared_from_this()));
}