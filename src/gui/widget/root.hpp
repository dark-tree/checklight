#pragma once

#include "panel.hpp"

/**
 * Root widget, all widgets in a GUI descend from it
 */
class RootWidget : public PanelWidget {

	private:

		std::shared_ptr<Overlay> over;
		int x, y;
		bool dirty = true;

	protected:

		void overlay(const std::shared_ptr<Overlay>& overlay) override;

	public:

		RootWidget();
		RootWidget(int x, int y);

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		bool event(WidgetContext& context, const InputEvent& event) override;

		/// Mark the GUI for rebuilding
		void update() override;

		/// Rebuild GUI layout IF the gui is newly created or it has been modified
		void rebuildLayout();

		/// Change the position of root element on screen
		void setOrigin(int x, int y);


};
