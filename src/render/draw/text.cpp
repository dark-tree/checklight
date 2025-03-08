
#include "text.hpp"

#include <render/asset/font.hpp>

/*
 * BakedText
 */

BakedText::BakedText(const std::vector<GlyphQuad>& quads, float baseline, float height)
: quads(quads), baseline(baseline), height(height) {}

const std::vector<GlyphQuad>& BakedText::getQuads() const {
	return quads;
}

GlyphQuad BakedText::getGlyph(int glyph) const {
	return quads[glyph];
}