
#include "widget.hpp"
#include "input/input.hpp"
#include "render/immediate.hpp"
#include "context.hpp"
#include "navigator.hpp"

/*
 * Widget
 */

void Widget::rebuild(int x, int y) {

	applyFitSizing(Channel::WIDTH);
	applyGrowSizing(Channel::WIDTH);
	applyFitSizing(Channel::HEIGHT);
	applyGrowSizing(Channel::HEIGHT);

	applyPositioning(x, y);
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
	}

	// ignore GROW sizing, that is handled in applyGrowSizing()

}

void Widget::applyGrowSizing(Channel channel) {

	const bool along = getFlowChannel(flow) == channel;
	const int spacing = gap.toPixels(); /* TODO ensure that gap is absolute */

	int remaining = sizing.get(channel);

	int total = 0;
	int fractions = 0;
	std::vector<std::pair<int, std::shared_ptr<Widget>>> growable;

	for (const std::shared_ptr<Widget>& widget : children) {
		Unit unit = (channel == Channel::WIDTH) ? widget->width : widget->height;
		const int outer = widget->getOuterSizing(channel);

		// subtract elements from out total size
		// to get at the still unused space
		remaining -= outer + spacing;

		// GROW elements will be saved for later use
		if (unit.metric == Metric::GROW) {

			if (along) {
				growable.emplace_back(unit.value, widget);
				fractions += unit.value;
				total += outer;
				continue;
			}

			// in case our element wants to grow across just set it
			// to 100% of the parent sizing component
			widget->sizing.get(channel) = sizing.get(channel);

		}
	}

	// remove trailing element gap
	if (!children.empty()) {
		remaining += spacing;
	}

	if (fractions < growable.size()) {
		out::error("Invalid fractional size found during grow apply, %d is less then the element count %d!", fractions, growable.size());
		fractions = growable.size();
	}

	if (!growable.empty() && remaining > 0) {

		// total space that can be taken by grow elements
		total += remaining;
		int part = total / fractions;
		int reminder = total % fractions;

		for (auto [fraction, widget] : growable) {
			int extension = fraction * part;

			if (reminder) {
				extension ++;
				reminder --;
			}

			// subtract old size and do it as an addition so we don't have to worry about padding
			widget->sizing.get(channel) += extension - widget->getOuterSizing(channel);
		}
	}

	for (const std::shared_ptr<Widget>& widget : children) {
		widget->applyGrowSizing(channel);
	}

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

void Widget::scan(Navigator& navigator) {
	for (auto& widget : children) {
		widget->scan(navigator);
	}
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

void InputWidget::scan(Navigator& navigator) {
	navigator.addWidget(std::dynamic_pointer_cast<InputWidget>(shared_from_this()));
	Widget::scan(navigator);
}