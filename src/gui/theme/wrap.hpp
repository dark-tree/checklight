#pragma once

#include <gui/theme.hpp>

class WrapTheme : public Theme {

	private:

		std::shared_ptr<Theme> theme;

	protected:

		void styleCommon(const std::shared_ptr<Widget>& widget) const override;
		void styleInput(const std::shared_ptr<InputWidget>& widget) const override;

		void styleButton(const std::shared_ptr<ButtonWidget>& widget) const override;
		void styleField(const std::shared_ptr<FieldWidget>& widget) const override;
		void stylePanel(const std::shared_ptr<PanelWidget>& widget) const override;
		void styleSelect(const std::shared_ptr<SelectWidget>& widget) const override;
		void styleSlider(const std::shared_ptr<SliderWidget>& widget) const override;
		void styleText(const std::shared_ptr<TextWidget>& widget) const override;
		void styleImage(const std::shared_ptr<ImageWidget>& widget) const override;

	protected:

		~WrapTheme() override = default;

		void setTheme(const std::shared_ptr<Theme>& theme);

};
