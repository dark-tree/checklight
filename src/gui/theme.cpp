#include "theme.hpp"

std::shared_ptr<ButtonWidget> Theme::newButton() const {
	auto widget = std::make_shared<ButtonWidget>();
	styleButton(widget);
	styleInput(widget);
	styleCommon(widget);
	return widget;
}

std::shared_ptr<FieldWidget> Theme::newTextField() const {
	auto widget = std::make_shared<FieldWidget>();
	styleField(widget);
	styleInput(widget);
	styleCommon(widget);
	return widget;
}

std::shared_ptr<PanelWidget> Theme::newPanel() const {
	auto widget = std::make_shared<PanelWidget>();
	stylePanel(widget);
	styleCommon(widget);
	return widget;
}

std::shared_ptr<SelectWidget> Theme::newSelect() const {
	auto widget = std::make_shared<SelectWidget>();
	styleSelect(widget);
	styleInput(widget);
	styleCommon(widget);
	return widget;
}

std::shared_ptr<SliderWidget> Theme::newSlider() const {
	auto widget = std::make_shared<SliderWidget>();
	styleSlider(widget);
	styleInput(widget);
	styleCommon(widget);
	return widget;
}

std::shared_ptr<TextWidget> Theme::newText() const {
	auto widget = std::make_shared<TextWidget>();
	styleText(widget);
	styleCommon(widget);
	return widget;
}

std::shared_ptr<ButtonWidget> Theme::newButton(const std::string& label) const {
	auto button = newButton();
	button->addWidget(newText(label));
	return button;
}

std::shared_ptr<FieldWidget> Theme::newTextField(const std::string& value) const {
	auto field = newTextField();
	field->setValue(value);
	return field;
}

std::shared_ptr<SelectWidget> Theme::newSelect(const std::vector<std::string>& labels) const {
	auto select = newSelect();
	select->setOptions(labels);
	return select;
}

std::shared_ptr<SelectWidget> Theme::newSelect(std::initializer_list<const char*> options) const {
	std::vector<std::string> labels;

	for (auto& option : options) {
		labels.emplace_back(option);
	}

	return newSelect(labels);
}

std::shared_ptr<SliderWidget> Theme::newSlider(float value) const {
	auto slider = newSlider();
	slider->setValue(value);
	return slider;
}

std::shared_ptr<SliderWidget> Theme::newSlider(float value, float step) const {
	auto slider = newSlider();
	slider->setValue(value);
	slider->setStep(step);
	return slider;
}

std::shared_ptr<TextWidget> Theme::newText(const std::string& value) const {
	auto text = newText();
	text->setText(value);
	return text;
}
