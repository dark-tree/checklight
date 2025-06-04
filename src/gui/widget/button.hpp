#pragma once

#include "gui/widget.hpp"

/**
 * Clickable button widget, can be equipped with a
 * on-click callback function
 */
class ButtonWidget : public InputWidget {

	public:

		using Callback = std::function<void()>;

	private:

		Callback callback;

	public:

		ButtonWidget();
		ButtonWidget(const std::string& label);
		ButtonWidget(const Callback& callback);
		ButtonWidget(const std::string& label, const Callback& callback);

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

		void addWidget(const std::shared_ptr<Widget>& widget);
		void onClick(const Callback& callback);

};