
#include "sprite.hpp"

Sprite::Sprite(float u1, float v1, float u2, float v2)
: u1(u1), v1(v1), u2(u2), v2(v2) {}

Sprite Sprite::shrink(float margin) {
	return {u1 + margin, v1 + margin, u2 - margin, v2 - margin};
}
