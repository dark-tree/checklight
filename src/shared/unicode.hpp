#pragma once

#include "external.hpp"

namespace utf8 {

	using UnicodeVector = std::vector<uint32_t>;

	/**
	 * Load the next Unicode character from the given UTF8 string,
	 * When zero is returned the sequence is over, and calling loadNext will cause undefined behaviour.
	 *
	 * @param utf8   pointer to an utf8 encoded c-string
	 * @param offset initially pointer to zero, then used by loadNext as an offset into the string
	 */
	uint32_t loadNext(const char* utf8, int* offset);

	/**
	 * Convert a standard utf encoded c-string into a vector of 43 bit Unicode code points.
	 */
	UnicodeVector toCodePoints(const char* utf8);

}