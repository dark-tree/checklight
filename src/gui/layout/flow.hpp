#pragma once

#include "channel.hpp"

enum struct Flow : int {
	LEFT_TO_RIGHT = 0b10,
	RIGHT_TO_LEFT = 0b11,
	TOP_TO_BOTTOM = 0b00,
	BOTTOM_TO_TOP = 0b01,
};

constexpr Channel getFlowChannel(Flow flow) {
	return (int(flow) & 0b10) ? Channel::WIDTH : Channel::HEIGHT;
}

constexpr int getFlowDirection(Flow flow) {
	return (int(flow) & 0b01) ? -1 : +1;
}