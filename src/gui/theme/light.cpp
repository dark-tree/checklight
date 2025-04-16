#include "light.hpp"

void LightTheme::styleCommon(const std::shared_ptr<Widget>& widget) const {
	widget->radius = Unit::px(6);

	widget->background.transition(100ms, ease::ofInOut) = [] (const ElementState& state) noexcept -> Color {
		if (state.interaction == ElementState::PRESSED) return {210, 210, 230};
		if (state.interaction == ElementState::HOVER) return {200, 200, 220};

		return {180, 180, 200};
	};

	widget->border_color = Color {40, 40, 80};

	widget->border.transition(100ms, ease::ofInOut) = [] (const ElementState& state) noexcept -> Unit {
		return state.focused ? Unit::px(4) : Unit::px(0);
	};
}

void LightTheme::styleInput(const std::shared_ptr<InputWidget>& widget) const {
	widget->min_height = Unit::px(30);
}

void LightTheme::styleButton(const std::shared_ptr<ButtonWidget>& widget) const {
	widget->min_width = Unit::px(100);
	widget->vertical = VerticalAlignment::CENTER;
	widget->horizontal = HorizontalAlignment::CENTER;
}

void LightTheme::styleField(const std::shared_ptr<FieldWidget>& widget) const {
	widget->min_width = Unit::px(100 - 8);
	widget->vertical = VerticalAlignment::CENTER;
	widget->horizontal = HorizontalAlignment::LEFT;

	widget->padding = BoxUnit {Unit::zero(), Unit::zero(), Unit::px(8), Unit::zero()};
}

void LightTheme::stylePanel(const std::shared_ptr<PanelWidget>& widget) const {
	widget->background = Color {238, 238, 255};
}

void LightTheme::styleSelect(const std::shared_ptr<SelectWidget>& widget) const {
	widget->min_width = Unit::px(100 - 8);

	widget->option_color = [] (const ElementState& state) noexcept -> Color {
		if (state.interaction == ElementState::PRESSED) return {210, 210, 240};
		if (state.interaction == ElementState::HOVER) return {200, 200, 230};

		return {140, 140, 180};
	};

	widget->vertical = VerticalAlignment::CENTER;
	widget->horizontal = HorizontalAlignment::LEFT;
	widget->padding = BoxUnit {Unit::zero(), Unit::zero(), Unit::px(8), Unit::zero()};

	widget->arrow.transition(100ms) = [] (const ElementState& state) noexcept -> Color {
		return state.isActive() ? Color {10, 10, 10} : Color {40, 40, 40};
	};
}

void LightTheme::styleSlider(const std::shared_ptr<SliderWidget>& widget) const {
	widget->min_width = Unit::px(100);
	widget->knob_border_color = Color {40, 40, 80};
	widget->border = Unit::zero();
	widget->knob_size = Unit::px(7);

	widget->knob_border.transition(100ms, ease::ofInOut) = [] (const ElementState& state) noexcept -> Unit {
		return state.focused ? Unit::px(4) : Unit::px(0);
	};
}

void LightTheme::styleText(const std::shared_ptr<TextWidget>& widget) const {
}

void LightTheme::styleImage(const std::shared_ptr<ImageWidget>& widget) const {
}

