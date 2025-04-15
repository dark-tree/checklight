
#include "text.hpp"

#include <render/system.hpp>
#include <render/asset/font.hpp>

/*
 * BakedText
 */

BakedText::BakedText(const std::vector<GlyphQuad>& quads, Metrics metrics)
: quads(quads), metrics(metrics) {}

const std::vector<GlyphQuad>& BakedText::getQuads() const {
	return quads;
}

const BakedText::Metrics& BakedText::getMetrics() const {
	return metrics;
}

/*
 * BreakTracker
 */

void BreakTracker::set(std::vector<GlyphQuad>& quads, int offset) {
	this->previous = quads.size() - 1;
	this->offset = offset;
}

bool BreakTracker::has() const {
	return previous != -1;
}

int BreakTracker::apply(std::vector<GlyphQuad>& quads) {
	const int last = quads.size() - 1;
	int count = last - previous;
	previous = -1;

	while (count > 0) {
		quads.pop_back();
		count --;
	}

	return offset;
}

/*
 * TextBakery
 */

glm::vec2 TextBakery::getTextOffset(std::vector<GlyphQuad>& quads, int start, int end, float vertical) const {
	float horizontal = 0;

	// calculate text width
	for (int i = start; i < end; i++) {
		horizontal += quads[i].advance;
	}

	// convert alignment enums to factors
	const float mx = toAlignmentFactor(this->horizontal);
	const float my = toAlignmentFactor(this->vertical);

	// bounding box attachment position
	const float bx = width * mx;
	const float by = height * my;

	return glm::vec2 {bx - horizontal * mx, by - vertical * my};
}

int TextBakery::getWrappingBound() const {
	return wrapping ? width : std::numeric_limits<int>::max();
}


TextBakery& TextBakery::setFont(const std::string& path) {

	if (!RenderSystem::system) {
		FAULT("Can't request font '", path, "', the renderer has net been initialized yet!");
	}

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
		FAULT("No font set!");
	}

	float horizontal = 0;
	float line_width = 0;

	// holds previous Unicode (or 0)
	// used for querying kerning pairs
	uint32_t prev = 0;

	// used for wrapping and manual line breaks
	const float sx = x;
	const int wrap = getWrappingBound();

	std::vector<TextLine> lines;
	std::vector<GlyphQuad> adjusted;
	float scale = size / float(Font::resolution);

	BreakTracker breaker;

	// sussy creepers
	float spacing = size * Font::fraction;
	float vertical = spacing;
	y += size / 2;

	const auto submit = [&] {
		const int start = lines.empty() ? 0 : lines.back().end;

		if (adjusted.empty()) {
			return;
		}

		lines.emplace_back(start, adjusted.size());

		if (line_width > horizontal) {
			horizontal = line_width;
		}
	};

	const auto advance = [&] {
		submit();

		y += spacing;
		vertical += spacing;
		x = sx;
		prev = 0;
	};

	for (int i = 0; i < (int) unicodes.size(); i++) {
		const uint32_t unicode = unicodes[i];

		if (unicode == ' ') {
			breaker.set(adjusted, i);
		}

		GlyphQuad q = font->getOrLoad(&x, &y, scale, unicode, prev);
		prev = unicode;

		if (unicode == '\n') {
			advance();
			line_width = 0;
			continue;
		}

		if (line_width > wrap && breaker.has()) {

			// remove characters that will now be moved to the next line
			// we will need to re-emit them, a bit wasteful but simple to implement
			i = breaker.apply(adjusted);

			advance();
			line_width = 0;
			continue;
		}

		line_width += q.advance;
		adjusted.push_back(q);
	}

	// handle last line
	submit();

	// now apply alignment
	for (auto [start, end] : lines) {
		glm::vec2 offset = getTextOffset(adjusted, start, end, vertical);

		for (int i = start; i < end; i++) {
			adjusted[i].applyOffset(offset);
		};
	}

	return {adjusted, BakedText::Metrics {horizontal, vertical}};
}