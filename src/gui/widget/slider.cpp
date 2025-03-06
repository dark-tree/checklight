
#include "slider.hpp"

#include <input/event/button.hpp>
#include <input/event/frame.hpp>
#include <input/event/mouse.hpp>
#include <render/immediate.hpp>

SliderWidget::SliderWidget(const std::function<void()>& callback)
: enabled(true), pressed(false), callback(callback) {}

glm::vec2 SliderWidget::getKnobPosition() {
	return {x + value * w, y + h / 2};
}

void SliderWidget::draw(ImmediateRenderer& immediate) {

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

bool SliderWidget::handle(const InputEvent& any) {

	if (!enabled) {
		return false;
	}

	const glm::vec2 knob = getKnobPosition();

	// update mouse icon
	if (auto* event = any.as<FrameEvent>()) {
		if (hovered) event->cursor(CursorIcon::POINTER);
		if (pressed) event->cursor(CursorIcon::HORIZONTAL);
		return false;
	}

	if (auto* positioned = any.as<PositionedEvent>()) {

		const float range = knob_size + 6; // interaction margin
		const float half = range / 2;
		hovered = positioned->isWithinBox(knob.x - half, knob.y - half, range, range);

		if (auto* button = positioned->as<ButtonEvent>()) {
			if (button->isWithinBox(x, y, w, h) || pressed || hovered) {
				if (button->isPressEvent()) {
					pressed = true;
				}

				if (button->isReleaseEvent()) {
					pressed = false;
				}
			}
		}

		if (pressed) {
			value = (std::clamp(positioned->x, static_cast<double>(x), static_cast<double>(x) + w) - x) / w;
		}

		return false;
	}

	return false;
}