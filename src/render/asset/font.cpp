
#include "font.hpp"
#include "sprite.hpp"

#include "msdfgen.h"
#include "msdfgen-ext.h"
#include <ft2build.h>
#include FT_FREETYPE_H

/*
 * GlyphQuad
 */

bool GlyphQuad::shouldDraw() const {
	return true;
}

/*
 * Font
 */

static msdfgen::FreetypeHandle* freetype = msdfgen::initializeFreetype();

FT_Face Font::getFreeType() {
	// get around the fact there is no official way to access the underlying freetype font object
	return *reinterpret_cast<FT_Face*>(font);
}

bool Font::loadUnicode(uint32_t unicode, float scale, float range) {
	msdfgen::Shape shape;
	GlyphInfo info;

	if (loadGlyph(shape, font, unicode, msdfgen::FONT_SCALING_EM_NORMALIZED, &info.advance)) {
		shape.normalize();

		info.advance *= scale;
		auto box = shape.getBounds();

		float tx = (1 - (box.r - box.l)) * 0.5f - box.l;
		float ty = (1 - (box.t - box.b)) * 0.5f - box.b;

		info.xoff = (-tx) * scale;
		info.yoff = ty * scale;

		edgeColoringSimple(shape, 3.0);

		ImageData image = ImageData::allocate(resolution, resolution, 4);
		msdfgen::Bitmap<float, 3> msdf(image.width(), image.height());

		msdfgen::SDFTransformation transform {
		msdfgen::Projection(scale, msdfgen::Vector2(tx, ty)),
		msdfgen::Range(range) / scale
		};

		msdfgen::generateMSDF(msdf, shape, transform);

		// convert to some normal format...
		for (int x = 0; x < msdf.width(); x++) {
			for (int y = 0; y < msdf.height(); y++) {
				float* pixel = msdf(x, y);
				uint8_t rgba[4];

				rgba[0] = msdfgen::pixelFloatToByte(pixel[0]);
				rgba[1] = msdfgen::pixelFloatToByte(pixel[1]);
				rgba[2] = msdfgen::pixelFloatToByte(pixel[2]);
				rgba[3] = msdfgen::pixelFloatToByte(pixel[3]);

				memcpy(image.pixel(x, y), rgba, 4);
			}
		}

		Sprite sprite = atlas->submit(image);
		image.close();

		info.x0 = sprite.u1;
		info.y0 = sprite.v2;
		info.x1 = sprite.u2;
		info.y1 = sprite.v1;

		cdata[unicode] = info;

		return true;
	}

	return false;
}

Font::Font(std::shared_ptr<DynamicAtlas> atlas, std::string path, int weight) {

	if (!freetype) {
		throw std::runtime_error {"Failed to initialize FreeType library!"};
	}

	this->font = msdfgen::loadFont(freetype, path.c_str());

	this->atlas = atlas;
	this->path = path;

	if (!font) {
		throw std::runtime_error {"Failed to open font: '" + path + "'"};
	}

	msdfgen::setFontVariationAxis(freetype, font, "Weight", weight);

	FT_Face face = getFreeType();
	printf("INFO: Loaded font '%s' from '%s'\n", face->family_name, path.c_str());
}

GlyphQuad Font::getOrLoad(float* x, float* y, float scale, uint32_t unicode, int prev) {

	GlyphQuad quad;

	double kerning = 0;

	if (prev != 0) {
		msdfgen::getKerning(kerning, font, prev, unicode, msdfgen::FONT_SCALING_EM_NORMALIZED);
	}

	auto pair = cdata.find(unicode);

	if (pair == cdata.end()) {
		loadUnicode(unicode, 64, 6);
		return getOrLoad(x, y, scale, unicode, prev);
	}

	GlyphInfo& info = pair->second;

	int round_x = (int) floor((*x + info.xoff * scale) + 0.5f);
	int round_y = (int) floor((*y + info.yoff * scale) + 0.5f);

	quad.x0 = round_x + kerning * 100 * scale;
	quad.y0 = round_y;
	quad.x1 = round_x + (info.x1 - info.x0) * scale;
	quad.y1 = round_y + (info.y1 - info.y0) * scale;

	quad.s0 = info.x0;
	quad.t0 = info.y1;
	quad.s1 = info.x1;
	quad.t1 = info.y0;

	*x += info.advance * scale;
	return quad;

}

/*
 * DynamicFontAtlas
 */

DynamicFontAtlas::DynamicFontAtlas(const std::shared_ptr<DynamicAtlas>& atlas)
: atlas(atlas) {}

std::shared_ptr<Font> DynamicFontAtlas::getOrLoad(const std::string& path) {
	auto it = fonts.find(path);

	if (it != fonts.end()) {
		return it->second;
	}

	auto font = std::make_shared<Font>(atlas, path, 500);
	fonts[path] = font;
	return font;
}