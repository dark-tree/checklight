#pragma once

#include "channel.hpp"

enum struct Flow : int {
	LEFT_TO_RIGHT = 0b10,
	RIGHT_TO_LEFT = 0b11,
	TOP_TO_BOTTOM = 0b00,
	BOTTOM_TO_TOP = 0b01,
};

struct WidgetFlow {

	/// Convert flow to it's primary axis
	static constexpr Channel  asChannel(Flow flow) {
		return (int(flow) & 0b10) ? Channel::WIDTH : Channel::HEIGHT;
	}

	/// Convert flow to it's direction (sign)
	static constexpr int asDirection(Flow flow) {
		return (int(flow) & 0b01) ? -1 : +1;
	}

	/// Check if the given flow is aligned with the axis
	static constexpr bool isAligned(Flow flow, Channel channel) {
		return asChannel(flow) == channel;
	}

};