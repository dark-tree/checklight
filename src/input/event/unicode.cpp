
#include "unicode.hpp"

/*
 * UnicodeEvent
 */

InputEvent::Type UnicodeEvent::getType() const {
	return UnicodeEvent::type;
}

UnicodeEvent::UnicodeEvent(uint32_t unicode, double x, double y)
: InputEvent(x, y), unicode(unicode) {}