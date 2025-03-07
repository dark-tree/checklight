#pragma once

#include "gui/widget.hpp"

class SelectWidget : public Widget {

	protected:

		static constexpr int OPTION_NONE = -2;
		static constexpr int OPTION_HEADER = -1;

		int value = -1;

		bool unrolled = false;
		bool enabled;
		bool pressed;

		int hovered = OPTION_NONE;

		std::function<void()> callback;

	public:

		SelectWidget(const std::function<void()>& callback);

		void draw(ImmediateRenderer &immediate) override;
		bool handle(const InputEvent &event) override;

};