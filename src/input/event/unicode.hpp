#pragma once

#include "positioned.hpp"

/**
 * Text input event, don't use for generic keyboard controls
 * @see KeyboardEvent
 */
class UnicodeEvent : public PositionedEvent {

	public:

		uint32_t unicode;

		UnicodeEvent(uint32_t unicode, double x, double y);

};
