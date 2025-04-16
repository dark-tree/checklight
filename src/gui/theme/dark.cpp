#include "dark.hpp"

void DarkTheme::styleCommon(const std::shared_ptr<Widget>& widget) const {
	widget->radius = Unit::px(6);

	widget->background.transition(100ms, ease::ofInOut) = [] (const ElementState& state) noexcept -> Color {
		if (state.interaction == ElementState::PRESSED) return {30, 30, 35};
		if (state.interaction == ElementState::HOVER) return {20, 20, 25};

		return {14, 14, 17};
	};

	widget->border_color = Color {80, 80, 200};

	widget->border.transition(100ms, ease::ofInOut) = [] (const ElementState& state) noexcept -> Unit {
		return state.focused ? Unit::px(4) : Unit::px(0);
	};
}

void DarkTheme::styleInput(const std::shared_ptr<InputWidget>& widget) const {
	widget->min_height = Unit::px(30);
}

void DarkTheme::styleButton(const std::shared_ptr<ButtonWidget>& widget) const {
	widget->min_width = Unit::px(100);
	widget->vertical = VerticalAlignment::CENTER;
	widget->horizontal = HorizontalAlignment::CENTER;
}

void DarkTheme::styleField(const std::shared_ptr<FieldWidget>& widget) const {
	widget->min_width = Unit::px(100 - 8);
	widget->vertical = VerticalAlignment::CENTER;
	widget->horizontal = HorizontalAlignment::LEFT;
	widget->color = Color {150, 150, 150};
	widget->placeholder_color = Color {60, 60, 60};

	widget->padding = BoxUnit {Unit::zero(), Unit::zero(), Unit::px(8), Unit::zero()};
}

void DarkTheme::stylePanel(const std::shared_ptr<PanelWidget>& widget) const {
	widget->background = Color {8, 8, 10};
}

void DarkTheme::styleSelect(const std::shared_ptr<SelectWidget>& widget) const {
	widget->min_width = Unit::px(100 - 8);
	widget->color = Color {150, 150, 150};
	widget->placeholder_color = Color {60, 60, 60};

	widget->option_color = [] (const ElementState& state) noexcept -> Color {
		if (state.interaction == ElementState::PRESSED) return {25, 25, 30};
		if (state.interaction == ElementState::HOVER) return {20, 20, 25};

		return {14, 14, 17};
	};

	widget->vertical = VerticalAlignment::CENTER;
	widget->horizontal = HorizontalAlignment::LEFT;
	widget->padding = BoxUnit {Unit::zero(), Unit::zero(), Unit::px(8), Unit::zero()};

	widget->arrow.transition(100ms) = [] (const ElementState& state) noexcept -> Color {
		return state.isActive() ? Color {10, 10, 10} : Color {40, 40, 40};
	};
}

void DarkTheme::styleSlider(const std::shared_ptr<SliderWidget>& widget) const {
	widget->min_width = Unit::px(100);
	widget->knob_border_color = Color {80, 80, 200};
	widget->border = Unit::zero();
	widget->knob_size = Unit::px(7);

	widget->knob_border.transition(100ms, ease::ofInOut) = [] (const ElementState& state) noexcept -> Unit {
		return state.focused ? Unit::px(4) : Unit::px(0);
	};
}

void DarkTheme::styleText(const std::shared_ptr<TextWidget>& widget) const {
	widget->color = Color {150, 150, 150};
}

