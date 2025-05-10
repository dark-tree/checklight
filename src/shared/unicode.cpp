
#include "unicode.hpp"

uint32_t utf8::loadNext(const char* utf8, int* offset) {

	const auto* ptr = reinterpret_cast<const unsigned char*>(utf8 + *offset);

	// this function can read past the end of the given string
	// if it's invalidly formatted, but at most by 3 bytes
	if (*ptr == 0) {
		return 0;
	}

	// 1-byte character: 0xxxxxxx
	if (*ptr < 0x80) {
		*offset += 1;
		return *ptr;
	}

	// 2-byte character: 110xxxxx 10xxxxxx
	if ((*ptr & 0xE0) == 0xC0) {
		*offset += 2;
		return (*ptr & 0x1F) << 6 | (*(ptr + 1) & 0x3F);
	}

	// 3-byte character: 1110xxxx 10xxxxxx 10xxxxxx
	if ((*ptr & 0xF0) == 0xE0) {
		*offset += 3;
		return (*ptr & 0x0F) << 12 | (*(ptr + 1) & 0x3F) << 6 | (*(ptr + 2) & 0x3F);
	}

	// 4-byte character: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	if ((*ptr & 0xF8) == 0xF0) {
		*offset += 4;
		return (*ptr & 0x07) << 18 | (*(ptr + 1) & 0x3F) << 12 | (*(ptr + 2) & 0x3F) << 6 | (*(ptr + 3) & 0x3F);
	}

	// Invalid
	*offset += 1;
	return 0;
}

utf8::UnicodeVector utf8::toCodePoints(const char* utf8) {
	UnicodeVector points;
	int offset = 0;

	while (true) {
		uint32_t unicode = utf8::loadNext(utf8, &offset);

		if (unicode == 0) {
			break;
		}

		points.push_back(unicode);
	}

	return points;
}
