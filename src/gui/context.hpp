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

		/// Focuses the widget for keyboard navigation
		void setSelected(const std::shared_ptr<InputWidget>& widget);

		/// Sets the given widget as the root of thw widget tree and recomputes navigator
		void setRoot(const std::shared_ptr<Widget>& widget);

		/// Draw the widget using the given immediate
		void draw(ImmediateRenderer& immediate);

		/// Pass an event for processing into the GUI widget tree
		InputResult onEvent(const InputEvent& any) override;

};

