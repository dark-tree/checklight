#pragma once

#include "external.hpp"
#include "input/listener.hpp"

class ImmediateRenderer;
class InputWidget;
class Widget;

class WidgetContext : public InputListener {

	private:

		/// Pointer to the selected element.
		std::shared_ptr<InputWidget> selected = nullptr;

		/// The root element of this GUI tree, most likely a PanelWidget
		std::shared_ptr<Widget> root;

		/// Recursively scan widget tree for all selectable elements
		std::vector<std::shared_ptr<InputWidget>> getSelectableWidgets() const;

	public:

		void setSelected(const std::shared_ptr<InputWidget>& widget);
		void setRoot(const std::shared_ptr<Widget>& widget);

		void draw(ImmediateRenderer& immediate);
		InputResult onEvent(const InputEvent& any) override;

};

