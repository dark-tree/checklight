#pragma once

#include "panel.hpp"

class RootWidget : public PanelWidget {

	private:

		int x, y;
		bool dirty = true;

	public:

		RootWidget();
		RootWidget(int x, int y);

		void draw(ImmediateRenderer& immediate, ElementState state) override;

		/// Mark the GUI for rebuilding
		void update() override;

		/// Rebuild GUI layout IF the gui is newly created or it has been modified
		void rebuildLayout();

		/// Change the position of root element on screen
		void setOrigin(int x, int y);


};
