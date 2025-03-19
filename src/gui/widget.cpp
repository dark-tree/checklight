
#include "widget.hpp"
#include "input/input.hpp"
#include "render/immediate.hpp"
#include "context.hpp"

/*
 * Spacing
 */

Box2D Spacing::apply(int width, int height, const Box2D& initial) {
	return initial.expand(
		top.resolve(height),
		bottom.resolve(height),
		left.resolve(width),
		right.resolve(width)
	);
}

Box2D Spacing::remove(int width, int height, const Box2D& initial) {
	return initial.expand(
		-top.resolve(height),
		-bottom.resolve(height),
		-left.resolve(width),
		-right.resolve(width)
	);
}

/*
 * Widget
 */

void Widget::setBounds(Box2D bounds) {

	Box2D inherent = getInherentBox();
	Box2D intermediate = padding.apply(inherent.w, inherent.h, inherent);

	// calculate width in pixels based on parent
	int explicit_width = width.resolve(bounds.w);
	int explicit_height = height.resolve(bounds.h);

	// width, height control the padded-box (css border box)
	padded = intermediate.reserve(explicit_width, explicit_height);

	// undo the padding to get the final content box
	content = padding.remove(padded.w, padded.h, padded);

	// finally calculate the outer element bounds
	margined = margin.apply(bounds.w, bounds.w /* intentionally */, padded);

	// needed to later offset content and padded
	const float rx = margined.x;
	const float ry = margined.y;

	// align margined
	const int cx = std::max(bounds.x, bounds.x + static_cast<int>((bounds.w - margined.w) * toAlignmentFactor(this->horizontal)));
	const int cy = std::max(bounds.y, bounds.y + static_cast<int>((bounds.h - margined.h) * toAlignmentFactor(this->vertical)));

	margined.x = cx;
	margined.y = cy;

	padded.x = padded.x - rx + cx;
	padded.y = padded.y - ry + cy;

	content.x = content.x - rx + cx;
	content.y = content.y - ry + cy;

}

Box2D Widget::getRemainingBox(const Box2D& bounds, const Widget* child) const {
	Display effective = child->getEffectiveDisplay();

	const int ew = child->margined.x - bounds.x + child->margined.w;
	const int eh = child->margined.y - bounds.y + child->margined.h;

	if (effective == Display::GREEDY) return {bounds.x, bounds.y + eh, 0, 0};
	if (effective == Display::VERTICAL) return {bounds.x + ew, bounds.y, std::max(0, bounds.w - ew), bounds.h};
	if (effective == Display::HORIZONTAL) return {bounds.x, bounds.y + eh, bounds.w, std::max(0, bounds.h - eh)};

	// AUTO display is resolved by getEffectiveDisplay()
	UNREACHABLE;
}

Display Widget::getEffectiveDisplay() const {
	if (display != Display::AUTO) {
		return display;
	}

	bool vertical = (this->vertical != VerticalAlignment::TOP);
	bool horizontal = (this->horizontal != HorizontalAlignment::LEFT);

	if (vertical && horizontal) return Display::GREEDY;
	if (horizontal) return Display::HORIZONTAL;

	// We treat vertical a bit like INLINE,
	// so just return that both when the element is vertical and when there is no alignment
	return Display::VERTICAL;
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