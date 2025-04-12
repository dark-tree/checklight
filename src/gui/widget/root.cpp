#include "root.hpp"

RootWidget::RootWidget()
: RootWidget(0, 0) {}

RootWidget::RootWidget(int x, int y) {
	setOrigin(x, y);
}

void RootWidget::draw(ImmediateRenderer& immediate, ElementState state) {
	rebuildLayout();
	PanelWidget::draw(immediate, state);
}

void RootWidget::update() {
	this->dirty = true;
}

void RootWidget::rebuildLayout() {
	if (dirty) {
		rebuild(x, y);
		dirty = false;
	}
}

void RootWidget::setOrigin(int x, int y) {
	this->x = x;
	this->y = y;
	this->dirty = true;
}
