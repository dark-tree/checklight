
#include "panel.hpp"

#include <render/immediate.hpp>

PanelWidget::PanelWidget() {

}

void PanelWidget::draw(ImmediateRenderer& immediate, ElementState state) {
	drawBasicPanel(immediate, state);

	for (auto& widget : children) {
		widget->draw(immediate, state);
	}
}

bool PanelWidget::event(WidgetContext& context, const InputEvent& event) {
	bool used = false;

	for (auto& widget : children) {
		used |= widget->event(context, event);
	}

	return used;
}

void PanelWidget::addWidget(const std::shared_ptr<Widget>& widget) {
	add(widget);
}


