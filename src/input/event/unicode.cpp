
#include "unicode.hpp"

/*
 * UnicodeEvent
 */

UnicodeEvent::UnicodeEvent(uint32_t unicode, double x, double y)
: PositionedEvent(x, y), unicode(unicode) {}