#pragma once

#include <optional>

#include "gui/widget.hpp"
#include "render/draw/text.hpp"
#include "shared/unicode.hpp"

class TextWidget : public Widget {

	public: // FIXME

		bool wrap = true;
		std::string text;

		void applyWrapSizing() override;

		TextBakery getBakery(int width, int height) const;

	public:

		TextWidget(const std::string& text);

		void draw(ImmediateRenderer& immediate) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

		void setText(const std::string& text);

};