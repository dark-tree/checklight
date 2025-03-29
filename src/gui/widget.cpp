
#include "widget.hpp"
#include "input/input.hpp"
#include "render/immediate.hpp"
#include "context.hpp"

/*
 * Widget
 */

void Widget::rebuild(int x, int y) {

	applyFitSizing(Channel::WIDTH);
	applyGrowSizing(Channel::WIDTH, sizing.width());
	applyFitSizing(Channel::HEIGHT);
	applyGrowSizing(Channel::HEIGHT, sizing.height());

	applyPositioning(x, y);
}

Box2D Widget::getInherentBox() const {
	return {0, 0, 100, 100};
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

int Widget::getOuterSizing(Channel channel) {
	return sizing.get(channel) + padding.left.toPixels() + padding.right.toPixels();
}

void Widget::applyFitSizing(Channel channel) {

	// fit sizing must be computed bottom-up
	// but even for absolute sizing we still need to call applyFitSizing on the children
	for (const std::shared_ptr<Widget>& widget : children) {
		widget->applyFitSizing(channel);
	}

	Unit unit = (channel == Channel::WIDTH) ? width : height;

	// handle the simple case - size is specified explicitly
	if (unit.isAbsolute()) {
		sizing.get(channel) = unit.toPixels();
		return;
	}

	// try to fit children along channel
	if (unit.metric == Metric::FIT) {
		int value = 0;

		const bool along = getFlowChannel(flow) == channel;
		const int spacing = gap.toPixels(); /* TODO ensure that gap is absolute */

		// get widths of all children
		for (const std::shared_ptr<Widget>& widget : children) {
			int inherent = widget->getOuterSizing(channel);

			value = along
				? value + inherent + spacing     // along flow direction
				: std::max(value, inherent); // acros flow direction
		}

		// remove trailing element gap
		if (along) {
			value -= gap.toPixels();
		}

		sizing.get(channel) = value;
		return;
	}

	if (unit.metric == Metric::GROW) {
		FAULT("GROW sizing not yet implemented");
	}

}

void Widget::applyGrowSizing(Channel channel, int parent) {

	// TODO

}

void Widget::applyPositioning(int x, int y) {

	// TODO maybe rename it to vector?
	Sizing position {x, y};

	content = Box2D {x, y, sizing.width(), sizing.height()};

	// TODO
	int pl = padding.left.toPixels();
	int pt = padding.top.toPixels();
	int pw = pl + padding.right.toPixels();
	int ph = pt + padding.bottom.toPixels();

	padded = Box2D {x - pl, y - pt, sizing.width() + pw, sizing.height() + ph};

	// this is used to effectively change the iteration direction
	const bool invert = getFlowDirection(flow) == -1;
	const Channel channel = getFlowChannel(flow);

	for (int i = 0; i < children.size(); i++) {
		const std::shared_ptr<Widget>& widget = children[invert ? (children.size() - i - 1) : i];
		widget->applyPositioning(position.width() /* x */, position.height() /* y */);
		position.get(channel) += widget->getOuterSizing(channel) + gap.toPixels();
	}

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