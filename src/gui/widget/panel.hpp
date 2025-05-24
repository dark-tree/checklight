#pragma once

#include "gui/widget.hpp"

class PanelWidget : public Widget {

	public:

		PanelWidget();

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

		void addWidget(const std::shared_ptr<Widget>& widget);

};