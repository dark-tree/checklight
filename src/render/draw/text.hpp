#pragma once

#include "external.hpp"

#include "render/asset/font.hpp"

class BakedText {

	private:

		std::vector<GlyphQuad> quads;

	public:

		float baseline, height;

		BakedText() = default;
		BakedText(const std::vector<GlyphQuad>& quads, float baseline, float height);

		/// Get all underlying text quads
		const std::vector<GlyphQuad>& getQuads() const;

		/// Get a specific glyph
		GlyphQuad getGlyph(int glyph) const;

};

class TextBakery {



};

