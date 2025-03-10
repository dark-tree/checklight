
#include "button.hpp"

#include "input/input.hpp"
#include "render/immediate.hpp"
#include "gui/context.hpp"


ButtonWidget::ButtonWidget(const std::string &label, const std::function<void()>& callback)
: InputWidget(), callback(callback), label(label) {}

void ButtonWidget::draw(ImmediateRenderer& immediate) {

	if (isFocused()) {
		immediate.setRectRadius(5);
		immediate.setColor(255, 255, 0);
		immediate.drawRect2D(padded.expand(8, 8, 8, 8));
	} else {
		pressed = false;
	}

	if (enabled) {
		if (hovered || pressed) {
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

	immediate.setRectRadius(10);
	immediate.drawRect2D(padded);

	immediate.setFont("assets/font/OpenSans-Variable.ttf");
	immediate.setColor(0, 0, 0);
	immediate.setTextBox(content.w, content.h);
	immediate.drawString2D(content.x, content.y, label);
}

bool ButtonWidget::event(WidgetContext& context, const InputEvent& any) {

	if (!enabled) {
		return false;
	}

	if (auto* event = any.as<FrameEvent>()) {
		if (hovered) {
			event->cursor(CursorIcon::POINTER);
			return true;
		}

		return false;
	}

	bool used = InputWidget::event(context, any);

	if (!isFocused()) {
		return false;
	}

	if (auto* button = any.as<ButtonEvent>()) {
		if (button->isReleaseEvent() && button->isWithinBox(padded)) {
			callback();
			return true;
		}
	}

	if (auto* keyboard = any.as<KeyboardEvent>()) {
		if (keyboard->wasPressed(GLFW_KEY_ENTER)) {
			pressed = true;
			return true;
		}

		if (keyboard->wasReleased(GLFW_KEY_ENTER)) {
			callback();
			pressed = false;
			return true;
		}
	}

	return used;
}