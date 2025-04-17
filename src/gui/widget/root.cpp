#include "root.hpp"

void RootWidget::overlay(const std::shared_ptr<Overlay>& overlay) {
	if (over) {
		over->bind(false);
	}

	over = overlay;
	over->bind(true);
}

RootWidget::RootWidget()
: RootWidget(0, 0) {}

RootWidget::RootWidget(int x, int y) {
	setOrigin(x, y);
}

void RootWidget::draw(ImmediateRenderer& immediate, ElementState state) {
	rebuildLayout();
	PanelWidget::draw(immediate, state);

	if (over) {
		if (over->shouldUnbind()) {
			over->bind(false);
			over = nullptr;
		} else {
			immediate.synchronize();
			over->draw(immediate);
		}
	}
}

bool RootWidget::event(WidgetContext& context, const InputEvent& event) {
	if (over) {
		if (over->shouldUnbind()) {
			over->bind(false);
			over = nullptr;
		} else if (over->event(context, event)) {
			return true;
		}
	}

	return PanelWidget::event(context, event);
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
