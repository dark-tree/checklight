#pragma once

#include <optional>

#include "gui/widget.hpp"
#include "render/draw/text.hpp"
#include "shared/unicode.hpp"

class PanelWidget : public Widget {

	public: // FIXME

		uint8_t r = 200;
		uint8_t g = 195;
		uint8_t b = 180;

	public:

		PanelWidget();

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

		void addWidget(const std::shared_ptr<Widget>& widget);

};