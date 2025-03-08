
#include "slider.hpp"

#include "gui/context.hpp"
#include "input/input.hpp"
#include "render/immediate.hpp"

SliderWidget::SliderWidget(const std::function<void()>& callback)
: InputWidget(), callback(callback) {}

void SliderWidget::updateValue() {
	if (value > 1.0) value = 1.0;
	if (value < 0.0) value = 0.0;

	if (step != 0.0) {
		const int count = round(value / step);
		value = count * step;
	}
}

glm::vec2 SliderWidget::getKnobPosition() {
	return {x + value * w, y + h / 2};
}

void SliderWidget::draw(ImmediateRenderer& immediate) {

	if (isFocused()) {
		immediate.setRectRadius(5);
		immediate.setColor(255, 255, 0);
		immediate.drawRect2D(x - 8, y - 8, w + 16, h + 16);
	}

	glm::vec2 knob = getKnobPosition();

	// background
	immediate.setColor(255, 0, 0);
	immediate.setRectRadius(10);
	immediate.drawRect2D(x, y, w, h);

	// slider rail
	immediate.setColor(0, 255, 0);
	immediate.drawRect2D(x, knob.y - rail_size / 2, w, rail_size);

	// slider knob
	immediate.setColor(0, 0, 255);
	immediate.drawCircle2D(knob.x, knob.y, knob_size);
}

bool SliderWidget::event(WidgetContext& context, const InputEvent& any) {

	if (!enabled) {
		return false;
	}

	const glm::vec2 knob = getKnobPosition();

	// update mouse icon
	if (const auto* event = any.as<FrameEvent>()) {
		if (hovered) {
			event->cursor(CursorIcon::POINTER);
			return true;
		}

		if (pressed) {
			event->cursor(CursorIcon::HORIZONTAL);
			return true;
		}

		return false;
	}

	if (auto* positioned = any.as<PositionedEvent>()) {

		const float range = knob_size + 6; // interaction margin
		const float half = range / 2;
		hovered = positioned->isWithinBox(knob.x - half, knob.y - half, range, range);
		bool used = false;

		if (const auto* button = positioned->as<ButtonEvent>()) {
			if (button->isWithinBox(x, y, w, h) || pressed || hovered) {
				if (button->isPressEvent()) {
					pressed = true;
					setFocus(context);
				}

				if (button->isReleaseEvent()) {
					pressed = false;
				}

				used = true;
			}
		}

		if (const auto* keyboard = any.as<KeyboardEvent>()) {
			if (!isFocused()) {
				return false;
			}

			if (keyboard->wasTyped(GLFW_KEY_LEFT)) {
				value -= step != 0.0f ? step : 0.1;
				updateValue();
				return true;
			}

			if (keyboard->wasTyped(GLFW_KEY_RIGHT)) {
				value += step != 0.0f ? step : 0.1;
				updateValue();
				return true;
			}
		}

		if (pressed) {
			value = (std::clamp(positioned->x, static_cast<double>(x), static_cast<double>(x) + w) - x) / w;
			updateValue();
		}

		return used;
	}

	return false;
}