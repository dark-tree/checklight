
#include "button.hpp"

#include "text.hpp"
#include "input/input.hpp"
#include "render/immediate.hpp"
#include "gui/context.hpp"


ButtonWidget::ButtonWidget()
: InputWidget() {

	this->border_color = Color {40, 40, 80};

	this->border.transition(100ms, ease::ofInOut) = [] (const ElementState& state) noexcept -> Unit {
		return state.focused ? Unit::px(4) : Unit::px(0);
	};

	this->radius = Unit::px(4);

	this->color = [] (const ElementState& state) noexcept -> Color {
		if (state.interaction == ElementState::PRESSED) return {220, 220, 240};
		if (state.interaction == ElementState::HOVER) return {200, 200, 220};

		return {180, 180, 200};
	};

	this->padding = BoxUnit {Unit::px(2)};

}

ButtonWidget::ButtonWidget(const std::string& label)
: ButtonWidget() {
	addWidget(std::make_shared<TextWidget>(label));
}

ButtonWidget::ButtonWidget(const Callback& callback)
: ButtonWidget() {
	onClick(callback);
}

ButtonWidget::ButtonWidget(const std::string& label, const Callback& callback)
: ButtonWidget() {
	addWidget(std::make_shared<TextWidget>(label));
	onClick(callback);
}

void ButtonWidget::draw(ImmediateRenderer& immediate, ElementState state) {

	if (!isFocused()) {
		pressed = false;
	}

	state = computeWidgetState();
	drawBasicPanel(immediate, state);

	for (auto& widget : children) {
		widget->draw(immediate, state);
	}

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

void ButtonWidget::addWidget(const std::shared_ptr<Widget>& widget) {
	add(widget);
}

void ButtonWidget::onClick(const std::function<void()>& callback) {
	this->callback = callback;
}