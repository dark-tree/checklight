#pragma once
#include "external.hpp"
#include "gui.hpp"

struct Theme {

	protected:

		// shared styles
		virtual void styleCommon(const std::shared_ptr<Widget>& widget) const = 0;
		virtual void styleInput(const std::shared_ptr<InputWidget>& widget) const = 0;

		// element specific styles
		virtual void styleButton(const std::shared_ptr<ButtonWidget>& widget) const = 0;
		virtual void styleField(const std::shared_ptr<FieldWidget>& widget) const = 0;
		virtual void stylePanel(const std::shared_ptr<PanelWidget>& widget) const = 0;
		virtual void styleSelect(const std::shared_ptr<SelectWidget>& widget) const = 0;
		virtual void styleSlider(const std::shared_ptr<SliderWidget>& widget) const = 0;
		virtual void styleText(const std::shared_ptr<TextWidget>& widget) const = 0;

	public:

		virtual ~Theme() = default;

		std::shared_ptr<ButtonWidget> newButton() const;
		std::shared_ptr<ButtonWidget> newButton(const std::string& label) const;
		std::shared_ptr<FieldWidget> newTextField() const;
		std::shared_ptr<FieldWidget> newTextField(const std::string& value) const;
		std::shared_ptr<PanelWidget> newPanel() const;
		std::shared_ptr<SelectWidget> newSelect() const;
		std::shared_ptr<SelectWidget> newSelect(const std::vector<std::string>& labels) const;
		std::shared_ptr<SelectWidget> newSelect(std::initializer_list<const char*> options) const ;
		std::shared_ptr<SliderWidget> newSlider() const;
		std::shared_ptr<SliderWidget> newSlider(float value) const;
		std::shared_ptr<SliderWidget> newSlider(float value, float step) const;
		std::shared_ptr<TextWidget> newText() const;
		std::shared_ptr<TextWidget> newText(const std::string& value) const;

};
