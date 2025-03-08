#pragma once

#include "gui/widget.hpp"

class SliderWidget : public InputWidget {

	private:

		float value = 0;

		std::function<void()> callback;

		float step = 0.1;
		float knob_size = 8;
		float rail_size = 3;

		glm::vec2 getKnobPosition();

	public:

		SliderWidget(const std::function<void()>& callback);

		void draw(ImmediateRenderer& immediate) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

};