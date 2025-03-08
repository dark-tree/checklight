#pragma once

#include "gui/widget.hpp"

class ButtonWidget : public Widget {

	private:

		std::function<void()> callback;
		std::string label;

	public:

		ButtonWidget(const std::string &label, const std::function<void()>& callback);

		void draw(ImmediateRenderer& immediate) override;
		bool handle(const InputEvent &event) override;

};