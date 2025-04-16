#include "wrap.hpp"

void WrapTheme::styleCommon(const std::shared_ptr<Widget>& widget) const {
	theme->styleCommon(widget);
}

void WrapTheme::styleInput(const std::shared_ptr<InputWidget>& widget) const {
	theme->styleInput(widget);
}

void WrapTheme::styleButton(const std::shared_ptr<ButtonWidget>& widget) const {
	theme->styleButton(widget);
}

void WrapTheme::styleField(const std::shared_ptr<FieldWidget>& widget) const {
	theme->styleField(widget);
}

void WrapTheme::stylePanel(const std::shared_ptr<PanelWidget>& widget) const {
	theme->stylePanel(widget);
}

void WrapTheme::styleSelect(const std::shared_ptr<SelectWidget>& widget) const {
	theme->styleSelect(widget);
}

void WrapTheme::styleSlider(const std::shared_ptr<SliderWidget>& widget) const {
	theme->styleSlider(widget);
}

void WrapTheme::styleText(const std::shared_ptr<TextWidget>& widget) const {
	theme->styleText(widget);
}

void WrapTheme::styleImage(const std::shared_ptr<ImageWidget>& widget) const {
	theme->styleImage(widget);
}

void WrapTheme::setTheme(const std::shared_ptr<Theme>& theme) {
	this->theme = theme;
}
