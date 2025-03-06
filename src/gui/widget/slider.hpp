#pragma once

#include "gui/widget.hpp"

class SliderWidget : public Widget {

	protected:

		float value = 0;

		bool enabled;
		bool hovered;
		bool pressed;

		std::function<void()> callback;
		float knob_size = 8;
		float rail_size = 3;

		glm::vec2 getKnobPosition();

	public:

		SliderWidget(const std::function<void()>& callback);

		void draw(ImmediateRenderer &immediate) override;
		bool handle(const InputEvent &event) override;

};