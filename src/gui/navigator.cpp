#include "navigator.hpp"

/*
 * Navigator
 */

void Navigator::addWidget(const std::shared_ptr<InputWidget>& widget) {
	selectable.emplace_back(widget);
}

void Navigator::next() {
	int i = 0;

	// find the currently selected element
	if (selected) {
		auto it = std::find(selectable.begin(), selectable.end(), selected);

		if (it != selectable.end()) {
			i = std::distance(selectable.begin(), it) + 1;

			// clamp to array size
			if (i >= (int) selectable.size()) {
				i = -1;
			}
		}
	}

	setSelected(i == -1 ? nullptr : selectable[i]);
}

void Navigator::setSelected(NULLABLE const std::shared_ptr<InputWidget>& widget) {
	if (selected) {
		selected->setSelected(false);
	}

	selected = widget;

	if (widget) {
		widget->setSelected(true);
	}
}

NULLABLE std::shared_ptr<InputWidget> Navigator::getSelected() const {
	return selected;
}
