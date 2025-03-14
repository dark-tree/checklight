#pragma once

#include <optional>

#include "gui/widget.hpp"
#include "render/draw/text.hpp"
#include "shared/unicode.hpp"

class TextWidget : public Widget {

	public: // FIXME

		std::string text;

	public:

		TextWidget(const std::string& text);

		void draw(ImmediateRenderer& immediate) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

		virtual Box2D getInherentBox() const;
		virtual void setBounds(Box2D bounds);

};