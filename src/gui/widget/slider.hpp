#pragma once

#include "gui/widget.hpp"

class SliderWidget : public InputWidget {

	private:

		float previous = 0;
		float value = 0;
		float step = 0;

		std::function<void(float)> callback;

		/// Updates the slider position to fall in valid range, and on a valid step
		void updateValue();

		/// Gets the knob x position in pixels based on slider value given
		glm::vec2 getKnobPosition(float value);

	public:

		/// Radius of the slider knob
		StyleProperty<Unit> knob_size = Unit::px(8);

		/// Color of the slider knob
		StyleProperty<Color> knob_color = Color {200, 200, 200};

		/// Border width of the slider knob
		StyleProperty<Unit> knob_border = Unit::zero();

		/// Border color of the slider knob
		StyleProperty<Color> knob_border_color = Color {0, 0, 0};

		/// Width of the slider knob rail
		StyleProperty<Unit> rail_size = Unit::px(3);

		/// Color of the slider knob rail
		StyleProperty<Color> rail_color = Color {50, 50, 50};

		/// Color of the rail divot
		StyleProperty<Color> divot_color = Color {10, 10, 10};

		/// Divot extend from the rail
		StyleProperty<Unit> divot_extend = Unit::px(1);

	public:

		SliderWidget();
		SliderWidget(float value);
		SliderWidget(float value, float step);

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

		void onChange(const std::function<void(float)>& callback);
		float getValue() const;
		void setStep(float step);
		void setValue(float value);

};