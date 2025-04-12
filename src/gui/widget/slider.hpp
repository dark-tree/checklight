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

		StyleProperty<float> knob_size = 8;
		StyleProperty<float> rail_size = 3;

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