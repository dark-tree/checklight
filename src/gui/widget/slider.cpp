
#include "slider.hpp"

#include "gui/context.hpp"
#include "input/input.hpp"
#include "render/immediate.hpp"

SliderWidget::SliderWidget()
: SliderWidget(0) {}

SliderWidget::SliderWidget(float value)
: SliderWidget(value, 0) {}

SliderWidget::SliderWidget(float value, float step)
: InputWidget() {
	setValue(value);
	setStep(step);
}

void SliderWidget::updateValue() {
	if (value > 1.0) value = 1.0;
	if (value < 0.0) value = 0.0;

	if (step != 0.0) {
		const int count = round(value / step);
		value = count * step;
	}

	if (value == previous) {
		return;
	}

	if (callback) {
		callback(value);
	}

	previous = value;
}

glm::vec2 SliderWidget::getKnobPosition(float value) {
	return {content.x + value * content.w, content.y + content.h / 2};
}

void SliderWidget::draw(ImmediateRenderer& immediate, ElementState state) {

	state = computeWidgetState();
	glm::vec2 knob = getKnobPosition(value);

	drawBasicPanel(immediate, state);

	const int rail = rail_size.fetch(state).pixels();

	// slider rail
	if (rail) {

		int divots = 1.0f / step;
		float length = content.w;
		float interval = length / divots;
		float sy = knob.y - rail / 2;

		immediate.setFill(rail_color.fetch(state));
		immediate.setStroke(OFF);
		immediate.drawRect2D(content.x, sy, length, rail);

		// only draw divots if we have good enough spacing
		if (interval >= 8) {
			float offset = content.x;

			immediate.setFill(divot_color.fetch(state));
			immediate.setLineWidth(1);

			int extend = divot_extend.fetch(state).pixels();

			for (int i = 0; i < divots; i++) {
				immediate.drawLine2D(offset, sy - extend, offset, sy + rail + extend);
				offset += interval;
			}

			immediate.drawLine2D(offset, sy - extend, offset, sy + rail + extend);
		}

	}

	// slider knob
	immediate.setStrokeWidth(knob_border.fetch(state).pixels());
	immediate.setStroke(knob_border_color.fetch(state));
	immediate.setFill(knob_color.fetch(state));
	immediate.drawCircle2D(knob.x, knob.y, knob_size.fetch(state).pixels());
}

bool SliderWidget::event(WidgetContext& context, const InputEvent& any) {

	if (!enabled) {
		return false;
	}

	const glm::vec2 knob = getKnobPosition(value);

	// update mouse icon
	if (const auto* event = any.as<FrameEvent>()) {
		if (pressed) {
			event->cursor(CursorIcon::HORIZONTAL);
			return true;
		}

		if (hovered) {
			event->cursor(CursorIcon::POINTER);
			return true;
		}

		return false;
	}

	if (auto* positioned = any.as<PositionedEvent>()) {

		const float range = knob_size.fetch(computeWidgetState()).pixels() + 6; // interaction margin
		const float half = range / 2;
		hovered = positioned->isWithinBox(knob.x - half, knob.y - half, range, range);
		bool used = false;

		if (const auto* button = positioned->as<ButtonEvent>()) {
			if (button->isWithinBox(padded) || pressed || hovered) {
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
			double lo = content.x;
			double hi = content.x + content.w;

			value = (std::clamp(positioned->x, lo, hi) - content.x) / content.w;
			updateValue();
		}

		return used;
	}

	return false;
}

void SliderWidget::onChange(const std::function <void(float)>& callback) {
	this->callback = callback;
}

float SliderWidget::getValue() const {
	return value;
}

void SliderWidget::setStep(float step) {
	this->step = step;
}

void SliderWidget::setValue(float value) {
	this->value = value;
}
