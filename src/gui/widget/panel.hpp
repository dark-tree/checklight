#pragma once

#include <optional>

#include "gui/widget.hpp"
#include "render/draw/text.hpp"
#include "shared/unicode.hpp"

class PanelWidget : public Widget {

	public: // FIXME

		std::vector<std::shared_ptr<Widget>> children;

		uint8_t r = 200;
		uint8_t g = 195;
		uint8_t b = 180;

	public:

		PanelWidget();

		void draw(ImmediateRenderer& immediate) override;
		bool event(WidgetContext& context, const InputEvent &event) override;
		void appendSelectable(std::vector<std::shared_ptr<InputWidget>>& selectable) override;

		void addWidget(const std::shared_ptr<Widget>& widget);

		virtual Box2D getInherentBox() const;
		virtual void setBounds(Box2D bounds);

};