
#include "select.hpp"

#include <shared/math.hpp>

#include "input/input.hpp"
#include "render/immediate.hpp"
#include "gui/context.hpp"

SelectWidget::SelectWidget(const std::vector<std::string>& labels) {
	setOptions(labels);
	min_height = Unit::px(30);
	min_width = Unit::px(100);

	color = Color {100, 100, 100};
	radius = Unit::px(10);
	vertical = VerticalAlignment::CENTER;
	horizontal = HorizontalAlignment::CENTER;
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

	double speed = 8;
	double time = std::clamp((glfwGetTime() - cooldown) * speed, 0.0, 1.0);
	bool show = unrolled || (time <= 1.0);
	if (!unrolled) time = 1 - time;
	double delta = ease::ofInOut(time);

	const RadiusUnit corners = radius.fetch(state);
	const Color background = color.fetch(state);
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
	immediate.setFill(background);

	immediate.drawRect2D(padded);
	immediate.setFont("assets/font/OpenSans-Variable.ttf");

	if (value == -1) {
		immediate.setFill(20, 20, 20);
	} else {
		immediate.setFill(0, 0, 0);
	}

	immediate.setTextAlignment(horizontal.fetch(state));
	immediate.setTextAlignment(vertical.fetch(state));
	immediate.setTextBox(content.w, content.h);

	std::string header = value == -1 ? placeholder : options[value].label;
	immediate.drawString2D(content.x, content.y, header);

	if (!show) return;
	const auto alpha = static_cast<uint8_t>(delta * 255);

	immediate.setRectRadius(0);
	immediate.setFill(50, 50, 50, alpha);
	immediate.setLineWidth(1);
	immediate.drawLine2D(padded.x, padded.y + padded.h, padded.x + padded.w, padded.y + padded.h);

	// options
	for (int i = 0; i < (int) options.size(); i ++) {
		int oy = padded.y + padded.h + content.h * i;

		// background
		if (option == i) {
			if (pressed) {
				immediate.setFill(255, 100, 100, alpha);
			} else {
				immediate.setFill(255, 80, 80, alpha);
			}
		} else {
			immediate.setFill(255, 0, 0, alpha);
		}

		immediate.drawRect2D(padded.x, oy, padded.w, content.h);

		// content
		immediate.setFont("assets/font/OpenSans-Variable.ttf");
		immediate.setFill(0, 0, 0, alpha);
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