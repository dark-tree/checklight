#pragma once

#include "gui/widget.hpp"

class ButtonWidget : public InputWidget {

	private:

		std::function<void()> callback;

	public:

		ButtonWidget();

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

		void addWidget(const std::shared_ptr<Widget>& widget);
		void onClick(const std::function<void()>& callback);

};