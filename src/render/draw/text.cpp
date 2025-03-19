
#include "text.hpp"

#include <render/system.hpp>
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

/*
 * TextBakery
 */

glm::vec2 TextBakery::getTextOffset(std::vector<GlyphQuad>& quads, int start, int end, float vertical) const {
	float dx = 0;

	// TODO
	for (int i = start; i < end; i++) {
		dx += quads[i].advance;
	}

	// convert alignment enums to factors
	float mx = static_cast<int>(this->horizontal) / 2.0f;
	float my = 1 - (static_cast<int>(this->vertical) / 2.0f); // TODO

	float ex = width * mx;
	float ey = height * my;

	return glm::vec2 {ex - dx * mx, ey - vertical * my};
}

TextBakery& TextBakery::setFont(const std::string& path) {
	return setFont(RenderSystem::system->getAssetLoader().getFont(path));
}

TextBakery& TextBakery::setFont(const std::shared_ptr<Font>& font) {
	this->font = font;
	return *this;
}

TextBakery& TextBakery::setBounds(int width, int height) {
	this->width = width;
	this->height = height;
	return *this;
}

TextBakery& TextBakery::setSize(int size) {
	this->size = size;
	return *this;
}

TextBakery& TextBakery::setAlignment(VerticalAlignment alignment) {
	this->vertical = alignment;
	return *this;
}

TextBakery& TextBakery::setAlignment(HorizontalAlignment alignment) {
	this->horizontal = alignment;
	return *this;
}

TextBakery& TextBakery::setAlignment(HorizontalAlignment horizontal, VerticalAlignment vertical) {
	return setAlignment(horizontal).setAlignment(vertical);
}

TextBakery& TextBakery::setWrapping(bool wrap) {
	this->wrapping = wrap;
	return *this;
}

BakedText TextBakery::bakeString(float x, float y, const std::string& string) {
	return bakeUnicode(x, y, utf8::toCodePoints(string.c_str()));
}

BakedText TextBakery::bakeUnicode(float x, float y, const utf8::UnicodeVector& unicodes) {

	if (!font) {
		throw std::runtime_error {"No font set!"};
	}

	uint32_t prev = 0;

	float sx = x;

	std::vector<std::pair<int, int>> lines;
	std::vector<GlyphQuad> adjusted;
	float scale = size / 100.0f;
	//float baseline = y + alignment.y;

	int wrap = std::numeric_limits<int>::max();

	if (wrapping && width != 0) {
		wrap = sx + width;
	}

	int lbr = -1;
	int cnt = 0;

	auto submit = [&] {
		int start = lines.empty() ? 0 : lines.back().second;
		lines.emplace_back(start, adjusted.size());
	};

	float spacing = size * 0.8;
	float vertical = spacing;
	y += size/2;

	for (int i = 0; i < unicodes.size(); i++) {
		const uint32_t unicode = unicodes[i];

		if (unicode == ' ') {
			lbr = i;
			cnt = 0;
		}

		GlyphQuad q = font->getOrLoad(&x, &y, scale, unicode, prev);
		prev = unicode;

		if (q.x1 > wrap && (lbr != -1)) {
			while (cnt) {
				adjusted.pop_back();
				cnt --;
			}

			submit();

			i = lbr;
			lbr = -1;
			cnt = 0;

			y += spacing;
			vertical += spacing;
			x = sx;
			prev = 0;

			continue;
		}

		adjusted.push_back(q);
		cnt ++;
	}

	submit();

	for (auto [start, end] : lines) {
		glm::vec2 offset = getTextOffset(adjusted, start, end, vertical);

		for (int i = start; i < end; i++) {
			adjusted[i].applyOffset(offset);
		};
	}

	return {adjusted, 0, spacing};
}

