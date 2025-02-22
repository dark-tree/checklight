#pragma once

#include "external.hpp"

namespace utf8 {

	uint32_t loadNext(const char* utf8, int* offset);
	std::vector<uint32_t> toCodePoints(const char* utf8);

}