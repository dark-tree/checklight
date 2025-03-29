
#include "panel.hpp"

#include <render/immediate.hpp>

PanelWidget::PanelWidget() {

}

void PanelWidget::draw(ImmediateRenderer& immediate) {
	immediate.setRectRadius(10);
	immediate.setColor(r, g, b);
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

Box2D PanelWidget::getInherentBox() const {
	return {0, 0, 0, 0};
}

// void PanelWidget::setBounds(Box2D bounds) {
// 	// Widget::setBounds(bounds);
// 	// Box2D pool = content;
// 	//
// 	// for (auto& widget : children) {
// 	// 	widget->setBounds(pool);
// 	// 	pool = widget->getRemainingBox(pool, widget.get());
// 	// }
// }

void PanelWidget::appendSelectable(std::vector<std::shared_ptr<InputWidget>>& selectable) {
	for (auto& widget : children) {
		widget->appendSelectable(selectable);
	}
}

