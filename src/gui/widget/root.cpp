#include "root.hpp"

void RootWidget::draw(ImmediateRenderer& immediate, ElementState state) {
	if (dirty) {
		rebuild(0, 0);
		dirty = false;
	}

	PanelWidget::draw(immediate, state);
}

void RootWidget::update() {
	this->dirty = true;
}


