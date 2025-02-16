#pragma once

#include "input/event.hpp"

/**
 * Window resize input event
 */
class ResizeEvent : public InputEvent {

	public:

		static constexpr Type type = InputEvent::RESIZE;

		Type getType() const override;

	public:

		int width;
		int height;

		ResizeEvent(int width, int height, double x, double y);

};
