#pragma once

#include "external.hpp"
#include "navigator.hpp"
#include "input/listener.hpp"

class ImmediateRenderer;
class InputWidget;
class Widget;

class WidgetContext : public InputListener {

	private:

		/// Used for keyboard navigation of the UI
		Navigator navigator;

		/// The root element of this GUI tree, most likely a PanelWidget
		std::shared_ptr<Widget> root;

	public:

		void setSelected(const std::shared_ptr<InputWidget>& widget);
		void setRoot(const std::shared_ptr<Widget>& widget);

		void draw(ImmediateRenderer& immediate);
		InputResult onEvent(const InputEvent& any) override;

};

