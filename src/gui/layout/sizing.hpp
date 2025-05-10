#pragma once

#include "external.hpp"
#include "channel.hpp"

class Sizing {

	private:

		glm::ivec2 box;

	public:

		Sizing()
		: box({0, 0}) {}

		Sizing(int w, int h)
		: box(w, h) {}

	public:

		int& get(Channel channel) {
			return box[(int) channel];
		}

		int& width() {
			return get(Channel::WIDTH);
		}

		int& height() {
			return get(Channel::HEIGHT);
		}

};