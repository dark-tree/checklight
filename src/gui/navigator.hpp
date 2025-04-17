#pragma once

#include "widget.hpp"

class Navigator {

	private:

		/// Pointer to the selected element.
		std::shared_ptr<InputWidget> selected = nullptr;

		/// List of all selectable widgets, used for keyboard navigation
		std::vector<std::shared_ptr<InputWidget>> selectable;

	public:

		void addWidget(const std::shared_ptr<InputWidget>& widget);

		void next();
		void setSelected(NULLABLE const std::shared_ptr<InputWidget>& widget);
		NULLABLE std::shared_ptr<InputWidget> getSelected() const;

};
