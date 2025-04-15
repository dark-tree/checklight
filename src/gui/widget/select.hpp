#pragma once

#include "gui/widget.hpp"

class SelectWidget : public InputWidget {

	public:

		using Callback = std::function<void(int)>;

	private:

		struct Option {
			std::string label;

			Option(const std::string& label) : label(label) {}
		};

		static constexpr int OPTION_NONE = -1;

		int value = -1;

		double cooldown = 0;
		bool unrolled = false;
		int option = OPTION_NONE;

		std::string placeholder = "Select";
		std::vector<Option> options;
		Callback callback;

		/// Show or hide the dropdown menu
		void setUnrolled(bool unrolled);

		/// Change option value, and call callback if applicable
		void setValue(int value);

	public:

		/// Font size
		StyleProperty<int> size = 20;

		/// Separator width
		StyleProperty<Unit> separator = Unit::px(1);

		/// Separator color
		StyleProperty<Color> separator_color = Color {50, 50, 50};

		/// Text color
		StyleProperty<Color> color = Color {0, 0, 0};

		/// Placeholder color
		StyleProperty<Color> placeholder_color = Color {40, 40, 40};

		/// Option background color
		StyleProperty<Color> option_color = Color {40, 40, 40};

		/// Dropdown arrow color
		StyleProperty<Color> arrow = Color {0, 0, 0};

		/// Font filepath
		StyleProperty<std::string> font = std::string {"assets/font/OpenSans-Variable.ttf"};

	public:

		SelectWidget() = default;
		SelectWidget(const std::vector<std::string>& labels);

		void setOptions(const std::vector<std::string>& labels);
		int getSelected() const;
		void setSelected(int option);

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

		void onSelect(const Callback& callback);

};