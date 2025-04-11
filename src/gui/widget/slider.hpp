#pragma once

#include "gui/widget.hpp"

class SliderWidget : public InputWidget {

	private:

		float previous = 0;
		float value = 0;

		std::function<void(float)> callback;

		float step = 0.2;
		float knob_size = 8;
		float rail_size = 3;

		/// Updates the slider position to fall in valid range, and on a valid step
		void updateValue();

		/// Gets the knob x position in pixels based on slider value given
		glm::vec2 getKnobPosition(float value);

	public:

		SliderWidget();

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

		void onChange(const std::function<void(float)>& callback);

};