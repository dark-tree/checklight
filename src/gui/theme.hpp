#pragma once

#include "external.hpp"
#include "widget/button.hpp"
#include "widget/field.hpp"
#include "widget/image.hpp"
#include "widget/panel.hpp"
#include "widget/select.hpp"
#include "widget/slider.hpp"
#include "widget/root.hpp"
#include "widget/text.hpp"

/**
 * A base factory for creating GUI widgets in a specific style
 */
struct Theme {

	protected:

		friend class WrapTheme;

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
		virtual void styleImage(const std::shared_ptr<ImageWidget>& widget) const = 0;

	public:

		virtual ~Theme() = default;

		virtual std::shared_ptr<ButtonWidget> newButton() const;
		virtual std::shared_ptr<FieldWidget> newTextField() const;
		virtual std::shared_ptr<PanelWidget> newPanel() const;
		virtual std::shared_ptr<SelectWidget> newSelect() const;
		virtual std::shared_ptr<SliderWidget> newSlider() const;
		virtual std::shared_ptr<TextWidget> newText() const;
		virtual std::shared_ptr<ImageWidget> newImage() const;

		std::shared_ptr<ButtonWidget> newButton(const std::string& label) const;
		std::shared_ptr<FieldWidget> newTextField(const std::string& value) const;
		std::shared_ptr<SelectWidget> newSelect(const std::vector<std::string>& labels) const;
		std::shared_ptr<SelectWidget> newSelect(std::initializer_list<const char*> options) const ;
		std::shared_ptr<SliderWidget> newSlider(float value) const;
		std::shared_ptr<SliderWidget> newSlider(float value, float step) const;
		std::shared_ptr<TextWidget> newText(const std::string& value) const;
		std::shared_ptr<ImageWidget> newImage(const std::string& sprite) const;
		std::shared_ptr<RootWidget> newRoot(int x, int y) const;

};
