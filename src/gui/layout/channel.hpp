#pragma once

#include "external.hpp"

enum struct Channel : int {
	WIDTH = 0,
	HEIGHT = 1
};

struct WidgetChannel {

	/// Convert flow to it's primary axis
	static constexpr Channel getOpposite(Channel channel) {
		return (channel == Channel::HEIGHT) ? Channel::WIDTH : Channel::HEIGHT;
	}

};