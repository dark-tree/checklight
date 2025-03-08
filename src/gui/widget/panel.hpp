#pragma once

#include <optional>

#include "gui/widget.hpp"
#include "render/draw/text.hpp"
#include "shared/unicode.hpp"

class PanelWidget : public Widget {

	private:

		std::vector<std::shared_ptr<Widget>> children;

	public:

		PanelWidget();

		void draw(ImmediateRenderer& immediate) override;
		bool event(WidgetContext& context, const InputEvent &event) override;
		void appendSelectable(std::vector<std::shared_ptr<InputWidget>>& selectable) override;

		void addWidget(const std::shared_ptr<Widget>& widget);

};