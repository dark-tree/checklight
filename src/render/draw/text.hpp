#pragma once

#include "external.hpp"
#include "render/asset/font.hpp"
#include "shared/unicode.hpp"
#include "render/draw/alignment.hpp"

/**
 * Represents a baked array of text quads that is ready for rendering
 */
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

/**
 * Used to track possible wrapping break points
 */
class BreakTracker {

	private:

		int offset = -1;
		int previous = -1;

	public:

		/// Set a new breakpoint at the given offset
		void set(std::vector<GlyphQuad>& quads, int offset);

		/// Check if there is a breakpoint set
		bool has() const;

		/// Return to the previous breakpoint by dropping quads and returning the old code point offset
		[[nodiscard]] int apply(std::vector<GlyphQuad>& quads);

};

/**
 * A BakedText factory
 */
class TextBakery {

	private:

		struct TextLine {
			int start;
			int end;
		};

		std::shared_ptr<Font> font = nullptr;
		int width, height;
		int size;
		VerticalAlignment vertical;
		HorizontalAlignment horizontal;
		bool wrapping;

		glm::vec2 getTextOffset(std::vector<GlyphQuad>& quads, int start, int end, float vertical) const;
		int getWrappingBound(int start) const;

	public:

		TextBakery& setFont(const std::string& path);
		TextBakery& setFont(const std::shared_ptr<Font>& font);
		TextBakery& setBounds(int width, int height);
		TextBakery& setSize(int size);
		TextBakery& setAlignment(VerticalAlignment alignment);
		TextBakery& setAlignment(HorizontalAlignment alignment);
		TextBakery& setAlignment(HorizontalAlignment horizontal, VerticalAlignment vertical);
		TextBakery& setWrapping(bool wrap);

		BakedText bakeUnicode(float x, float y, const utf8::UnicodeVector& unicodes);
		BakedText bakeString(float x, float y, const std::string& string);

};

