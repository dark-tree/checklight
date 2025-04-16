
#include "widget.hpp"
#include "input/input.hpp"
#include "render/immediate.hpp"
#include "context.hpp"
#include "navigator.hpp"
#include "layout/channel.hpp"

/*
 * Widget
 */

void Widget::rebuild(int x, int y) {

	applyFitSizing(Channel::WIDTH);
	applyGrowSizing(Channel::WIDTH);
	applyWrapSizing();
	applyFitSizing(Channel::HEIGHT);
	applyGrowSizing(Channel::HEIGHT);
	applyPositioning(x, y);

}

void Widget::add(const std::shared_ptr<Widget>& child) {
	child->parent = weak_from_this();
	children.push_back(child);
}

void Widget::remove(const std::shared_ptr<Widget>& child) {
	auto it = std::find(children.begin(), children.end(), child);

	if (it != children.end()) {
		children.erase(it);
	}
}

float Widget::getAlignmentFactor(const ElementState& state, Channel channel) {
	return channel == Channel::WIDTH ? toAlignmentFactor(horizontal.fetch(state)) : toAlignmentFactor(vertical.fetch(state));
}

int Widget::getOuterSizing(const ElementState& state, Channel channel) {
	return sizing.get(channel) + padding.fetch(state).along(channel) + margin.fetch(state).along(channel);
}

void Widget::drawBasicPanel(ImmediateRenderer& immediate, const ElementState& state) {

	const RadiusUnit corners = radius.fetch(state);
	const Color color = background.fetch(state);
	const Color border = border_color.fetch(state);

	// reset state
	immediate.setSprite(OFF);

	// configure border and background radius
	immediate.setRectRadius(
		corners.top_left.pixels(),
		corners.top_right.pixels(),
		corners.bottom_left.pixels(),
		corners.bottom_right.pixels()
	);

	// border width
	const int width = this->border.fetch(state).pixels();
	immediate.setStrokeWidth(width);

	// set colors
	immediate.setStroke(border);
	immediate.setFill(color);

	// draw panel with border
	immediate.drawRect2D(padded);

}

void Widget::applyWrapSizing() {
	for (const std::shared_ptr<Widget>& widget : children) {
		widget->applyWrapSizing();
	}
}

void Widget::applyFitSizing(Channel channel) {

	// fit sizing must be computed bottom-up
	// but even for absolute sizing we still need to call applyFitSizing on the children
	for (const std::shared_ptr<Widget>& widget : children) {
		widget->applyFitSizing(channel);
	}

	const ElementState state = ElementState::ofLayout();
	const Unit sizing_unit = (channel == Channel::WIDTH) ? width.fetch(state) : height.fetch(state);
	const Unit minimal_unit = (channel == Channel::WIDTH) ? min_width.fetch(state) : min_height.fetch(state);

	int preferred = 0;
	int low_bound = minimal_unit.pixels();

	// handle the simple case - size is specified explicitly
	if (sizing_unit.isResolvable()) {
		preferred = std::max(sizing_unit.pixels(), low_bound);
	}

	// try to fit children along channel
	if (sizing_unit.metric == Metric::FIT) {
		int value = 0;

		const bool along = WidgetFlow::isAligned(flow.fetch(state), channel);
		const int spacing = gap.fetch(state).pixels();

		// get widths of all children
		for (const std::shared_ptr<Widget>& widget : children) {
			int inherent = widget->getOuterSizing(state, channel);

			value = along
				? value + inherent + spacing     // along flow direction
				: std::max(value, inherent); // acros flow direction
		}

		// remove trailing element gap
		if (along) {
			value -= spacing;
		}

		preferred = std::max(value, low_bound);
	}

	// ignore GROW sizing, that is handled in applyGrowSizing()

	// get the highest minimal size
	for (const std::shared_ptr<Widget>& widget : children) {
		int min = widget->minimal.get(channel);

		if (min > low_bound) {
			low_bound = min;
		}
	}

	// apply calculated size
	sizing.get(channel) = preferred;
	minimal.get(channel) = low_bound;

}

void Widget::applyGrowSizing(Channel channel) {

	const ElementState state = ElementState::ofLayout();
	const bool along = WidgetFlow::isAligned(flow.fetch(state), channel);
	const int spacing = gap.fetch(state).pixels();

	int remaining = sizing.get(channel);

	int total = 0;
	int fractions = 0;
	std::vector<std::pair<int, std::shared_ptr<Widget>>> growable;

	for (const std::shared_ptr<Widget>& widget : children) {
		Unit unit = (channel == Channel::WIDTH) ? widget->width.fetch(state) : widget->height.fetch(state);
		const int outer = widget->getOuterSizing(state, channel);

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

	if (fractions < (int) growable.size()) {
		out::error("Invalid fractional size found during grow apply, %d is less then the element count %d!", fractions, growable.size());
		fractions = (int) growable.size();
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
			widget->sizing.get(channel) += extension - widget->getOuterSizing(state, channel);
		}
	}

	// not enough space!
	if (remaining < 0) {

		bool notify = true;
		bool done = false;
		int overflow = -remaining;

		std::list<std::shared_ptr<Widget>> candidates;

		for (const std::shared_ptr<Widget>& widget : children) {
			candidates.push_back(widget);
		}

		// this loop will only in rare cases run more than once
		for (int i = 0; i < (int) children.size(); i ++) {

			// TODO yeah this is a royal mess
			total = 0;

			int shrinkable = 0;

			for (auto it = candidates.begin(); it != candidates.end();) {
				auto& widget = *it;

				const int value = widget->sizing.get(channel);
				const int maximum = value - widget->minimal.get(channel);

				if (maximum <= 0) {
					it = candidates.erase(it);
					continue;
				}

				shrinkable += maximum;
				total += value;
				std::advance(it, 1);
			}

			// not really an error but good to know
			if (notify && (shrinkable < overflow)) {
				out::debug("Can't shrink elements enough, shrinkable space is %dpx, while the overflow is %dpx!", shrinkable, overflow);
				notify = false;
			}

			if (candidates.empty()) {
				out::debug("No elements are left to shrink, bailing out with overflow of %dpx!", overflow);
				done = true;
				break;
			}

			out::debug("Will try to shrink %d elements of total size %dpx by %dpx, the shrinkable space is %dpx", candidates.size(), total, overflow, shrinkable);

			double sacrifice = std::min(overflow, shrinkable);
			double shrinkage = sacrifice / total;
			int shrunk = 0;

			for (const std::shared_ptr<Widget>& widget : candidates) {
				const int value = widget->sizing.get(channel);

				int pixels = std::max(1, static_cast<int>(value * shrinkage));
				int maximum = widget->sizing.get(channel) - widget->minimal.get(channel);

				// never shrink below minimum
				if (pixels > maximum) {
					pixels = maximum;
				}

				shrunk += pixels;
				widget->sizing.get(channel) -= pixels;

				// try not to over-shrink, can happen dues to our
				// little hack to for at least try shaving a single pixel each time
				if (shrunk >= sacrifice) {
					break;
				}
			}

			// we did the job, if this fails that means some min sizing shenanigans got in the way
			// and we need to reevaluate the sizing by re-running this loop
			if (shrunk >= sacrifice) {
				done = true;
				break;
			}

			overflow -= shrunk;
		}

		if (!done) {
			out::warn("Element shrink algorithm bailed out after iteration limit was reached!");
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

	const ElementState state = ElementState::ofLayout();
	const BoxUnit pad = padding.fetch(state);

	// TODO
	int pl = pad.left.pixels();
	int pt = pad.top.pixels();
	int pw = pl + pad.right.pixels();
	int ph = pt + pad.bottom.pixels();

	padded = Box2D {x - pl, y - pt, sizing.width() + pw, sizing.height() + ph};

	Flow flow_value = flow.fetch(state);

	// this is used to effectively change the iteration direction
	const int facing = WidgetFlow::asDirection(flow_value);
	const bool invert = facing == -1;

	const Channel channel = WidgetFlow::asChannel(flow_value);
	const Channel opposite = WidgetChannel::getOpposite(channel);

	int remaining_along = sizing.get(channel);
	int spacing = gap.fetch(state).pixels();

	std::vector<std::pair<int, std::shared_ptr<Widget>>> growable;

	// calculate along-axis space left
	for (const std::shared_ptr<Widget>& widget : children) {

		// subtract elements from out total size
		// to get at the still unused space
		remaining_along -= widget->getOuterSizing(state, channel) + spacing;
	}

	// remove trailing element gap
	if (!children.empty()) {
		remaining_along += spacing;
	}

	for (int i = 0; i < (int) children.size(); i++) {
		const std::shared_ptr<Widget>& widget = children[invert ? (children.size() - i - 1) : i];

		// number of pixels left across the flow
		const int remaining_across = sizing.get(opposite) - widget->getOuterSizing(state, opposite);

		// alignment factors
		int align_x = remaining_along * getAlignmentFactor(state, channel) ;
		int align_y = remaining_across * getAlignmentFactor(state, opposite);

		// align_x is actually align_along, this effectively converts from local coordinates
		if (channel != Channel::WIDTH) {
			std::swap(align_x, align_y);
		}

		const BoxUnit wp = widget->padding.fetch(state);
		const BoxUnit wm = widget->margin.fetch(state);

		// when positioning we take into account only the upper-left offsets
		const int ox = align_x + wp.left.pixels() + wm.left.pixels();
		const int oy = align_y + wp.top.pixels() + wm.top.pixels();

		widget->applyPositioning(ox + position.width() /* x */, oy + position.height() /* y */);
		position.get(channel) += widget->getOuterSizing(state, channel) + spacing;

	}

}

Widget::~Widget() {
	// no-op base virtual destructor
}

bool Widget::event(WidgetContext& context, const InputEvent& any) {
	if (const auto* event = any.as<PositionedEvent>()) {
		if (event->isWithinBox(padded)) {
			return true;
		}
	}

	return false;
}

void Widget::scan(Navigator& navigator) {
	for (auto& widget : children) {
		widget->scan(navigator);
	}
}

void Widget::update() {
	if (auto locked = parent.lock()) {
		locked->update();
		return;
	}

	out::error("UI update was lost during propagation! Is the RootWidget missing?");
}

void Widget::overlay(const std::shared_ptr<Overlay>& overlay) {
	if (auto locked = parent.lock()) {
		locked->overlay(overlay);
		return;
	}

	out::error("UI overlay was lost during propagation! Is the RootWidget missing?");
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

ElementState InputWidget::computeWidgetState() const {
	ElementState::Interaction interaction;

	if (enabled) {
		if (hovered || pressed) {
			if (pressed) {
				interaction = ElementState::PRESSED;
			} else {
				interaction = ElementState::HOVER;
			}
		} else {
			interaction = ElementState::DEFAULT;
		}
	} else {
		interaction = ElementState::DISABLED;
	}

	return ElementState::ofDraw(interaction, isFocused());
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