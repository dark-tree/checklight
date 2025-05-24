#pragma once

#include "input/event.hpp"

/**
 * Window resize input event
 */
class ResizeEvent : public InputEvent {

	public:

		int width;
		int height;

		ResizeEvent(int width, int height);

};
