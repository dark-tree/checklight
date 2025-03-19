#pragma once

namespace msdfgen {
	class FontHandle;
	struct FontMetrics;
}

typedef struct FT_FaceRec_* FT_Face;

#include "atlas.hpp"

struct GlyphInfo {
	double x0, y0, x1, y1;
	double xoff, yoff, advance;
};

struct GlyphQuad {
	float advance;

	float x0, y0, s0, t0;
	float x1, y1, s1, t1;

	/// can return false for glyphs with no visible elements
	/// (e.g. spaces) to optimize the emitted vertex count
	bool shouldDraw() const;

	void applyOffset(glm::vec2 vec);
};

class Font {

	public:

		static constexpr int resolution = 64;

	private:

		msdfgen::FontMetrics* metrics;
		msdfgen::FontHandle* font;
		std::shared_ptr<DynamicAtlas> atlas;
		std::string path;
		std::unordered_map<uint32_t, GlyphInfo> cdata;

		/// Helper to get the underlying FreeType handle
		FT_Face getFreeType();

		/// Loads MSDF map into atlas
		bool loadUnicode(uint32_t unicode, float scale, float range);

	public:

		Font(std::shared_ptr<DynamicAtlas> atlas, std::string path, int weight);
		~Font();

		float getLineHeight() const;

		GlyphQuad getOrLoad(float* x, float* y, float scale, uint32_t unicode, int prev);

};

class DynamicFontAtlas {

	private:

		std::shared_ptr<DynamicAtlas> atlas;
		std::unordered_map<std::string, std::shared_ptr<Font>> fonts;

	public:

		DynamicFontAtlas(const std::shared_ptr<DynamicAtlas>& atlas);

		std::shared_ptr<Font> getOrLoad(const std::string& path);

};

