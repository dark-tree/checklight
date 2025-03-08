#pragma once

#include "gui/widget.hpp"

class SelectWidget : public InputWidget {

	private:

		struct Option {
			std::string label;
		};

		static constexpr int OPTION_NONE = -1;

		int value = -1;

		bool unrolled = false;
		int option = OPTION_NONE;

		std::vector<Option> options;
		std::function<void()> callback;

	public:

		SelectWidget(const std::function<void()>& callback);

		void draw(ImmediateRenderer& immediate) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

};