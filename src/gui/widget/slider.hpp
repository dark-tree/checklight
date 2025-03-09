#pragma once

#include "gui/widget.hpp"

class SliderWidget : public InputWidget {

	private:

		float render = 0;
		float value = 0;

		std::function<void()> callback;

		float step = 0.2;
		float knob_size = 8;
		float rail_size = 3;

		void updateValue();
		glm::vec2 getKnobPosition(float value);

	public:

		SliderWidget(const std::function<void()>& callback);

		void draw(ImmediateRenderer& immediate) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

};