
#include "select.hpp"

#include <shared/math.hpp>

#include "input/input.hpp"
#include "render/immediate.hpp"
#include "gui/context.hpp"

SelectWidget::SelectWidget(const std::function<void()>& callback)
: InputWidget(), unrolled(false), callback(callback) {

	for (int i = 0; i < 5; i ++) {
		options.emplace_back("Option: " + std::to_string(i));
	}

}

void SelectWidget::setUnrolled(bool unrolled) {
	this->unrolled = unrolled;
	this->cooldown = glfwGetTime();
}

void SelectWidget::draw(ImmediateRenderer& immediate) {

	if (isFocused()) {
		immediate.setRectRadius(5);
		immediate.setColor(255, 255, 0);
		immediate.drawRect2D(x - 8, y - 8, w + 16, h + 16);
	} else {
		if (unrolled) {
			setUnrolled(false);
		}
	}

	if (enabled) {
		if (hovered) {
			if (pressed) {
				immediate.setColor(255, 100, 100);
			} else {
				immediate.setColor(255, 80, 80);
			}
		} else {
			immediate.setColor(255, 0, 0);
		}
	} else {
		immediate.setColor(70, 70, 70);
	}

	double speed = 8;
	double time = std::clamp((glfwGetTime() - cooldown) * speed, 0.0, 1.0);
	bool show = unrolled || (time <= 1.0);
	if (!unrolled) time = 1 - time;
	double delta = math::easeInOut(time);

	// background
	immediate.setRectRadius(10, (1 - delta) * 10);
	immediate.drawRect2D(x, y, w, h);

	immediate.setFont("assets/font/OpenSans-Variable.ttf");
	immediate.setColor(0, 0, 0);
	immediate.setTextBox(w, h);
	immediate.drawString2D(x, y, "Selected: " + std::to_string(value));

	if (!show) return;
	const auto alpha = static_cast<uint8_t>(delta * 255);

	immediate.setRectRadius(0);

	immediate.setColor(50, 50, 50, alpha);
	immediate.setLineWidth(1);
	immediate.drawLine2D(x, y + h, x + w, y + h);

	// options
	for (int i = 0; i < (int) options.size(); i ++) {
		int oy = y + h * (i + 1);

		// background
		if (option == i) {
			if (pressed) {
				immediate.setColor(255, 100, 100, alpha);
			} else {
				immediate.setColor(255, 80, 80, alpha);
			}
		} else {
			immediate.setColor(255, 0, 0, alpha);
		}

		immediate.drawRect2D(x, oy, w, h);

		// content
		immediate.setFont("assets/font/OpenSans-Variable.ttf");
		immediate.setColor(0, 0, 0, alpha);
		immediate.setTextBox(w, h);
		immediate.drawString2D(x, oy, options.at(i).label);
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
					value = option;
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
		hovered = positioned->isWithinBox(x, y, w, h);

		if (unrolled) {
			for (int i = 0; i < (int) options.size(); i ++) {
				int oy = y + h * (i + 1);
				bool entry = positioned->isWithinBox(x, oy, w, h);

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
					value = option;
					return true;
				}
			}
		}

	}

	return false;
}