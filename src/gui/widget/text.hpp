#pragma once

#include "gui/widget.hpp"
#include "render/draw/text.hpp"

class TextWidget : public Widget {

	public:

		// this should be private but we need it in tests
		// so leave it here for now

		/// Get internal configured text geometry bakery
		TextBakery getBakery(int width, int height, const ElementState& state) const;

	private:

		std::string text;

		void applyWrapSizing() override;
		void updateWidgetText(const std::string& text);

	public:

		/// Should the text wrap if there is not enough space?
		StyleProperty<bool> wrap = true;

		/// Font size
		StyleProperty<int> size = 20;

		/// Text color
		StyleProperty<Color> color = Color {0, 0, 0};

		/// Font filepath
		StyleProperty<std::string> font = std::string {"assets/font/OpenSans-Variable.ttf"};

	public:

		TextWidget();
		TextWidget(const std::string& text);

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

		void setText(const std::string& text);

};