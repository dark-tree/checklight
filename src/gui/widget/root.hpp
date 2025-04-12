#pragma once

#include "panel.hpp"

class RootWidget : public PanelWidget {

	private:

		bool dirty = true;

	public:

		void draw(ImmediateRenderer& immediate, ElementState state) override;

		/// Mark the GUI for rebuilding
		void update() override;

		/// Rebuild GUI layout IF the gui is newly created or it has been modified
		void rebuildLayout();

};
