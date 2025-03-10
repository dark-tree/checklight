
#include "panel.hpp"

#include <render/immediate.hpp>

PanelWidget::PanelWidget() {

}

void PanelWidget::draw(ImmediateRenderer& immediate) {
	immediate.setRectRadius(10);
	immediate.setColor(200, 195, 180);
	immediate.drawRect2D(padded);

	for (auto& widget : children) {
		widget->draw(immediate);
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
	children.emplace_back(widget);
}

void PanelWidget::appendSelectable(std::vector<std::shared_ptr<InputWidget>>& selectable) {
	for (auto& widget : children) {
		widget->appendSelectable(selectable);
	}
}

