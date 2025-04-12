#include "root.hpp"

void RootWidget::draw(ImmediateRenderer& immediate, ElementState state) {
	rebuildLayout();
	PanelWidget::draw(immediate, state);
}

void RootWidget::update() {
	this->dirty = true;
}

void RootWidget::rebuildLayout() {
	if (dirty) {
		rebuild(0, 0);
		dirty = false;
	}
}

