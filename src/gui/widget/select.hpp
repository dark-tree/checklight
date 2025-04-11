#pragma once

#include "gui/widget.hpp"

class SelectWidget : public InputWidget {

	private:

		struct Option {
			std::string label;
		};

		static constexpr int OPTION_NONE = -1;

		int value = -1;

		double cooldown = 0;
		bool unrolled = false;
		int option = OPTION_NONE;

		std::vector<Option> options;
		std::function<void()> callback;

		/// Show or hide the dropdown menu
		void setUnrolled(bool unrolled);

	public:

		SelectWidget();

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

};