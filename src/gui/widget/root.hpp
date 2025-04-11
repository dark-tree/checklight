#pragma once

#include "panel.hpp"

class RootWidget : public PanelWidget {

	private:

		bool dirty = true;

	public:

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		void update() override;

};
