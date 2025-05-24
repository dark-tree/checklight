
#include "select.hpp"

#include <shared/math.hpp>

#include "input/input.hpp"
#include "render/immediate.hpp"
#include "gui/context.hpp"

/*
 * DropdownOverlay
 */

DropdownOverlay::DropdownOverlay(SelectWidget* parent) noexcept {
	this->parent = parent;
}

void DropdownOverlay::draw(ImmediateRenderer& immediate) {
	parent->drawDropdown(immediate);

}

bool DropdownOverlay::event(WidgetContext& context, const InputEvent& event) {
	return parent->event(context, event);
}

/*
 * SelectWidget
 */

SelectWidget::SelectWidget() {
	this->dropdown = std::make_shared<DropdownOverlay>(this);
}

SelectWidget::SelectWidget(const std::vector<std::string>& labels)
: SelectWidget() {
	setOptions(labels);
}

void SelectWidget::setOptions(const std::vector<std::string>& labels) {
	this->options.clear();
	this->value = -1;

	for (const auto& label : labels) {
		options.emplace_back(label);
	}
}

int SelectWidget::getSelected() const {
	return this->value;
}

void SelectWidget::setSelected(int option) {
	this->value = option;
}

void SelectWidget::onSelect(const Callback& callback) {
	this->callback = callback;
}

void SelectWidget::setUnrolled(bool unrolled) {
	this->unrolled = unrolled;
	this->cooldown = glfwGetTime();

	if (unrolled) {
		overlay(dropdown);
	} else {
		dropdown->unbind();
	}
}

void SelectWidget::setValue(int value) {
	if (this->value == value) {
		return;
	}

	this->value = value;

	if (callback) {
		callback(value);
	}
}

void SelectWidget::draw(ImmediateRenderer& immediate, ElementState state) {

	if (!isFocused() && unrolled) {
		setUnrolled(false);
	}

	state = computeWidgetState();
	state.active = unrolled;

	double speed = 8;
	double time = std::clamp((glfwGetTime() - cooldown) * speed, 0.0, 1.0);
	bool show = unrolled || (time <= 1.0);
	if (!unrolled) time = 1 - time;
	double delta = ease::ofInOut(time);

	const RadiusUnit corners = radius.fetch(state);
	const Color color = background.fetch(state);
	const Color border = border_color.fetch(state);

	// configure border and background radius
	immediate.setRectRadius(
		corners.top_left.pixels(),
		corners.top_right.pixels(),
		corners.bottom_left.pixels() * (1 - delta),
		corners.bottom_right.pixels() * (1 - delta)
	);

	// border width
	const int width = this->border.fetch(state).pixels();
	immediate.setStrokeWidth(width);

	// background
	immediate.setStroke(border);
	immediate.setFill(color);

	immediate.drawRect2D(padded);
	immediate.setFont(font.fetch(state));

	float mx = content.x + content.w - content.h / 2;
	float my = content.y + content.h / 2;
	float extend = 4;

	immediate.setFill(arrow.fetch(state));
	immediate.drawTrig2D(mx - extend, my - extend, mx + extend, my - extend, mx, my + (extend + extend * 0.25f));

	if (value == -1) {
		immediate.setFill(placeholder_color.fetch(state));
	} else {
		immediate.setFill(this->color.fetch(state));
	}

	immediate.setTextAlignment(horizontal.fetch(state));
	immediate.setTextAlignment(vertical.fetch(state));
	immediate.setFontSize(size.fetch(state));
	immediate.setTextBox(content.w, content.h);

	std::string header = value == -1 ? placeholder : options[value].label;
	immediate.drawString2D(content.x, content.y, header);

	if (!show) return;
	alpha = static_cast<uint8_t>(delta * 255);

	// separator
	immediate.setFill(separator_color.fetch(state).multiplyAlpha(alpha));
	immediate.setLineWidth(separator.fetch(state).pixels());
	immediate.drawLine2D(padded.x, padded.y + padded.h, padded.x + padded.w, padded.y + padded.h);

}

void SelectWidget::drawDropdown(ImmediateRenderer& immediate) {

	immediate.setStroke(OFF);
	immediate.setRectRadius(0);

	for (int i = 0; i < (int) options.size(); i ++) {
		int oy = padded.y + padded.h + content.h * i;

		ElementState option_state = ElementState::ofDraw(ElementState::DEFAULT, false);

		// background
		if (option == i) {
			option_state = ElementState::ofDraw(pressed ? ElementState::PRESSED : ElementState::HOVER, false);
		}

		immediate.setFill(option_color.fetch(option_state).multiplyAlpha(alpha));
		immediate.drawRect2D(padded.x, oy, padded.w, content.h);

		// content
		immediate.setFont(font.fetch(option_state));
		immediate.setFill(this->color.fetch(option_state).multiplyAlpha(alpha));
		immediate.setTextBox(content.w, content.h);
		immediate.drawString2D(content.x, oy, options.at(i).label);
	}

}

bool SelectWidget::event(WidgetContext& context, const InputEvent& any) {

	if (!enabled) {
		return false;
	}

	if (auto* event = any.as<FrameEvent>()) {
		if (hovered || unrolled) {
			event->cursor(CursorIcon::POINTER);
			return true;
		}

		return false;
	}

	if (auto* keyboard = any.as<KeyboardEvent>()) {
		if (keyboard->isReleaseEvent() && isFocused()) {
			if (keyboard->keycode == GLFW_KEY_ESCAPE && unrolled) {
				setUnrolled(false);
				option = value;
				pressed = false;
				return true;
			}

			if (keyboard->keycode == GLFW_KEY_ENTER) {
				if (unrolled) {
					setValue(option);
				} else {
					option = value;

					if (option == OPTION_NONE) {
						option = 0;
					}
				}

				setUnrolled(!unrolled);
				pressed = unrolled;
				return true;
			}
		}

		if (keyboard->isTypedEvent()) {
			if (keyboard->keycode == GLFW_KEY_DOWN && unrolled) {
				option = std::min((int) options.size() - 1, option + 1);
				return true;
			}

			if (keyboard->keycode == GLFW_KEY_UP && unrolled) {
				option = std::max(0, option - 1);
				return true;
			}
		}

		return false;
	}

	if (auto* positioned = any.as<PositionedEvent>()) {
		hovered = positioned->isWithinBox(padded);

		if (unrolled) {
			for (int i = 0; i < (int) options.size(); i ++) {
				int oy = padded.y + padded.h + content.h * i;
				bool entry = positioned->isWithinBox(content.x, oy, content.w, content.h);

				if (entry) {
					option = i;
				}
			}
		}

		if (auto* button = positioned->as<ButtonEvent>()) {

			if (isFocused() || hovered) {

				// this is a bit of a hack as I couldn't get the
				// logic below to comply with my desires, the multitude
				// of flags and conditions here make this a real pain to deal with
				// so we just force 'false' artificially here to cancel selection
				if (!unrolled && !hovered && isFocused()) {
					return false;
				}

				if (button->isPressEvent()) {
					pressed = true;
					setFocus(context);
					return true;
				}

				if (button->isReleaseEvent()) {
					pressed = false;
					setUnrolled(hovered && !unrolled);
					setValue(option);
					return true;
				}
			}
		}

	}

	return false;
}