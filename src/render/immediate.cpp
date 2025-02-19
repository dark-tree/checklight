
#include "immediate.hpp"
#include "vulkan/command/recorder.hpp"
#include "asset/atlas.hpp"
#include "shared/unicode.hpp"

/*
 * VertexChannel
 */

VertexChannel::VertexChannel(const std::string& string) {
	buffer.setDebugName("Immediate " + string + " Vertex Channel");
}

void VertexChannel::close() {
	clear();
	buffer.close();
}

void VertexChannel::upload(CommandRecorder& recorder) {
	if (!empty()) {
		buffer.resize(vertices.size(), sizeof(element));
		buffer.writeToStaging(vertices.data(), vertices.size(), sizeof(element));
		buffer.flushStaging(recorder);
	}
}

bool VertexChannel::empty() const {
	return vertices.empty();
}

void VertexChannel::clear() {
	vertices.clear();
}

/*
 * ImmediateRenderer
 */

void ImmediateRenderer::upload(CommandRecorder& recorder) {
	basic.upload(recorder);
	text.upload(recorder);
	atlas->upload(recorder);

	if (mapping != 0) {
		throw std::runtime_error {"Texture mapping stack overflow!"};
	}

	recorder.memoryBarrier()
		.first(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT)
		.then(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)
		.byRegionBit()
		.done();
}

void ImmediateRenderer::close() {
	basic.close();
}

Texture& ImmediateRenderer::getAtlasTexture() {
	return atlas->getTexture();
}

void ImmediateRenderer::clear() {
	basic.clear();
	text.clear();
}

void ImmediateRenderer::vertex(float x, float y) {
	if (!mapping) {
		throw std::runtime_error {"No texture mapped!"};
	}

	float du = sprite.u2 - sprite.u1;
	float dv = sprite.v2 - sprite.v1;

	float u = sprite.u1 + (x - tx) / tw * du;
	float v = sprite.v1 + (y - ty) / th * dv;

	vertex(x, y, u, v);
}

void ImmediateRenderer::vertex(float x, float y, float u, float v) {
	vertex(x * iw - 1, y * ih - 1, layer, u, v);
}

void ImmediateRenderer::vertex(glm::vec2 pos, float u, float v) {
	vertex(pos.x, pos.y, u, v);
}

void ImmediateRenderer::vertex(float x, float y, float z, float u, float v) {
	basic.write(x, y, z, r, g, b, a, u, v);
}

float ImmediateRenderer::getBezierPoint(float a, float b, float c, float d, float t) {
	const float it = 1 - t;

	const float it2 = it * it;
	const float it3 = it * it2;

	const float t2 = t * t;
	const float t3 = t * t2;

	return a * it3 + 3 * it2 * t * b + 3 * it * t2 * c + t3 * d;
}

float ImmediateRenderer::getBezierTangent(float a, float b, float c, float d, float t) {
	const float it = 1 - t;

	const float it2 = it * it;
	const float t2 = t * t;

	return 3 * it2 * (b - a) + 6 * it * t * (c - b) + 3 * t2 * (d - c);
}

void ImmediateRenderer::pushTextureMap(float x, float y, float w, float h) {
	if (!mapping) {
		this->tx = x;
		this->ty = y;
		this->tw = w;
		this->th = h;
	}

	mapping ++;
}

void ImmediateRenderer::popTextureMap() {
	mapping --;
}

ImmediateRenderer::ImmediateRenderer()
: atlas(std::make_shared<DynamicAtlas>()), images(atlas), fonts(atlas) {
	setLayer(0);
	setColor(255, 255, 255);
	setResolution(1, 1);
	setLineWidth(4);
	setRectRadius(0);
	setFontSize(20);
}

Sprite ImmediateRenderer::getSprite(const std::string& path) {
	return images.getOrLoad(path);
}

void ImmediateRenderer::setLayer(uint32_t layer) {
	this->layer = 1.0f / (layer + 1);
}

void ImmediateRenderer::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

void ImmediateRenderer::setResolution(uint32_t width, uint32_t height) {
	this->iw = 2.0f / std::max(1u, width);
	this->ih = 2.0f / std::max(1u, height);
}

void ImmediateRenderer::setLineWidth(float width) {
	this->width = width;
}

void ImmediateRenderer::setRectRadius(float radius) {
	setRectRadius(radius, radius);
}

void ImmediateRenderer::setRectRadius(float top, float bottom) {
	setRectRadius(top, top, bottom, bottom);
}

void ImmediateRenderer::setRectRadius(float top_left, float top_right, float bottom_left, float bottom_right) {
	this->rtl = top_left;
	this->rtr = top_right;
	this->rbl = bottom_left;
	this->rbr = bottom_right;
}

void ImmediateRenderer::setSprite(Sprite sprite) {
	this->sprite = sprite;
}

void ImmediateRenderer::setSprite(const std::string& path) {
	setSprite(getSprite(path));
}

void ImmediateRenderer::setFont(const std::string& path) {
	this->font = fonts.getOrLoad(path);
}

void ImmediateRenderer::setFontSize(int size) {
	this->font_size = size;
}

void ImmediateRenderer::setFont(const std::string& path, int size) {
	setFont(path);
	setFontSize(size);
}

void ImmediateRenderer::drawRect2D(float x, float y, float w, float h) {

	pushTextureMap(x, y, w, h);

	glm::vec2 par {x + rbl, y - rbl + h};
	glm::vec2 pbr {x + w - rbr, y - rbr + h};
	glm::vec2 pcr {x + w - rtr, y + rtr};
	glm::vec2 pdr {x + rtl, y + rtl};

	drawArc2D(par.x, par.y, rbl, rbl, glm::radians(180.0f), -M_PI_2);
	drawArc2D(pbr.x, pbr.y, rbr, rbr, glm::radians(90.0f), -M_PI_2);
	drawArc2D(pcr.x, pcr.y, rtr, rtr, glm::radians(0.0f), -M_PI_2);
	drawArc2D(pdr.x, pdr.y, rtl, rtl, glm::radians(270.0f), -M_PI_2);

	// main rect body
	drawQuad2D(par.x, par.y, pbr.x, pbr.y, pcr.x, pcr.y, pdr.x, pdr.y);

	// beveled walls
	drawQuad2D(par.x, par.y, pdr.x, pdr.y, pdr.x - rtl, pdr.y, par.x - rbl, par.y);
	drawQuad2D(pbr.x, pbr.y, pcr.x, pcr.y, pcr.x + rtr, pcr.y, pbr.x + rbr, pbr.y);
	drawQuad2D(pcr.x, pcr.y, pdr.x, pdr.y, pdr.x, pdr.y - rtl, pcr.x, pcr.y - rtr);
	drawQuad2D(par.x, par.y, pbr.x, pbr.y, pbr.x, pbr.y + rbr, par.x, par.y + rbl);

	popTextureMap();

}

void ImmediateRenderer::drawLine2D(float x1, float y1, float x2, float y2) {

	glm::vec2 pa {x1, y1};
	glm::vec2 pb {x2, y2};

	glm::vec2 ab = pb - pa;
	glm::vec2 pp = glm::normalize(glm::vec2 {-ab.y, ab.x}) * width;

	vertex(pa + pp, sprite.u1, sprite.v1);
	vertex(pb - pp, sprite.u2, sprite.v2);
	vertex(pb + pp, sprite.u1, sprite.v2);

	vertex(pa + pp, sprite.u1, sprite.v1);
	vertex(pa - pp, sprite.u2, sprite.v1);
	vertex(pb - pp, sprite.u2, sprite.v2);

}

void ImmediateRenderer::drawSlantedLine2D(glm::vec2 p1, glm::vec2 d1, glm::vec2 p2, glm::vec2 d2) {
	float half = width * 0.5f;

	// Calculate the vector along the end side of a segment line
	glm::vec2 s1 = glm::vec2 {-d1.y, d1.x} * half;
	glm::vec2 s2 = glm::vec2 {-d2.y, d2.x} * half;

	// Vertices near the P1 point
	glm::vec2 a1 = p1 + s1;
	glm::vec2 a2 = p1 - s1;

	// Vertices near the P2 point
	glm::vec2 b1 = p2 + s2;
	glm::vec2 b2 = p2 - s2;

	vertex(a1.x, a1.y, sprite.u1, sprite.v1);
	vertex(a2.x, a2.y, sprite.u2, sprite.v1);
	vertex(b1.x, b1.y, sprite.u1, sprite.v2);

	vertex(b1.x, b1.y, sprite.u1, sprite.v2);
	vertex(a2.x, a2.y, sprite.u2, sprite.v1);
	vertex(b2.x, b2.y, sprite.u2, sprite.v2);
}

void ImmediateRenderer::drawArc2D(float x, float y, float hrad, float vrad, float start, float angle, ArcMode mode) {

	pushTextureMap(x - hrad, y - vrad, hrad * 2, vrad * 2);

	float extent = std::max(hrad, vrad);

	// https://stackoverflow.com/a/11774493
	float correctness = 1 - /* TODO: draw_quality */ 0.95f / extent;
	int sides = std::max(3, (int) ceil(abs(angle) / acos(2 * correctness * correctness - 1)));
	float step = angle / sides;

	for (int i = 0; i < sides; i ++) {
		float ax = x + hrad * cos(start + step * i);
		float ay = y + vrad * sin(start + step * i);

		float bx = x + hrad * cos(start + step * (i + 1));
		float by = y + vrad * sin(start + step * (i + 1));

		vertex(x, y);
		vertex(ax, ay);
		vertex(bx, by);
	}

	if (angle > M_PI && mode == OPEN_CHORD) {
		float ax = x + hrad * cos(start);
		float ay = y + vrad * sin(start);

		float bx = x + hrad * cos(start + angle);
		float by = y + vrad * sin(start + angle);

		vertex(x, y);
		vertex(ax, ay);
		vertex(bx, by);
	}

	popTextureMap();

}

void ImmediateRenderer::drawEllipse2D(float x, float y, float hrad, float vrad) {
	drawArc2D(x, y, hrad, vrad, 0, M_PI * 2);
}

void ImmediateRenderer::drawCircle2D(float x, float y, float radius) {
	drawEllipse2D(x, y, radius, radius);
}

void ImmediateRenderer::drawQuad2D(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {

	if (mapping) {
		vertex(x1, y1);
		vertex(x2, y2);
		vertex(x3, y3);

		vertex(x1, y1);
		vertex(x3, y3);
		vertex(x4, y4);

		return;
	}

	vertex(x1, y1, sprite.u1, sprite.v1);
	vertex(x2, y2, sprite.u2, sprite.v1);
	vertex(x3, y3, sprite.u2, sprite.v2);

	vertex(x1, y1, sprite.u1, sprite.v1);
	vertex(x3, y3, sprite.u2, sprite.v2);
	vertex(x4, y4, sprite.u1, sprite.v2);

}

void ImmediateRenderer::drawBezier2D(float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy) {

	float t = 0;

	bool draw = false;
	glm::vec2 last_point;
	glm::vec2 last_tangent;

	glm::vec2 p1 {ax, ay};
	glm::vec2 p2 {bx, by};
	glm::vec2 p3 {cx, cy};
	glm::vec2 p4 {dx, dy};

	// the curve will be no longer than this upper bound
	const float bound = glm::distance(p1, p2) + glm::distance(p2, p3) + glm::distance(p3, p4);

	// Quality control specifies the spacing between T values that are sampled
	float spacing = 10.0f + 60.0f * /* TODO: draw_quality */ 0.9f;
	float parts = bound / spacing;
	float segments = parts + (12 / parts + 3);
	float step = 1.0f / segments;

	// The last point is calculated outside the loop
	const float ex = getBezierTangent(ax, bx, cx, dx, 1.0f);
	const float ey = getBezierTangent(ay, by, cy, dy, 1.0f);

	while (t < 1.0f) {

		// Calculate point on the bezier curve
		const float px = getBezierPoint(ax, bx, cx, dx, t);
		const float py = getBezierPoint(ay, by, cy, dy, t);
		const glm::vec2 point {px, py};

		// Calculate the tangent at the same point
		const float tx = getBezierTangent(ax, bx, cx, dx, t);
		const float ty = getBezierTangent(ay, by, cy, dy, t);
		const glm::vec2 tangent = glm::normalize(glm::vec2 {tx, ty});

		if (draw) {
			drawSlantedLine2D(last_point, last_tangent, point, tangent);
		}

		last_point = point;
		last_tangent = tangent;
		draw = true;
		t += step;
	}

	// Handle the endpoint
	glm::vec2 point {dx, dy};
	glm::vec2 tangent = glm::normalize(glm::vec2 {ex, ey});
	drawSlantedLine2D(last_point, last_tangent, point, tangent);

}

void ImmediateRenderer::drawText2D(float x, float y, const std::string& str) {

	if (!font) {
		throw std::runtime_error {"No font set!"};
	}

	uint32_t unicode = 0;
	int prev = 0;
	int offset = 0;

	while (true) {
		prev = unicode;
		unicode = loadNextUnicode(str.c_str(), &offset);

		if (unicode == 0) {
			break;
		}

		GlyphQuad q = font->getOrLoad(&x, &y, font_size / 100.0f, unicode, prev);

		text.write(q.x0 * iw - 1, q.y1 * ih - 1, layer, r, g, b, a, q.s0, q.t1);
		text.write(q.x0 * iw - 1, q.y0 * ih - 1, layer, r, g, b, a, q.s0, q.t0);
		text.write(q.x1 * iw - 1, q.y0 * ih - 1, layer, r, g, b, a, q.s1, q.t0);

		text.write(q.x0 * iw - 1, q.y1 * ih - 1, layer, r, g, b, a, q.s0, q.t1);
		text.write(q.x1 * iw - 1, q.y0 * ih - 1, layer, r, g, b, a, q.s1, q.t0);
		text.write(q.x1 * iw - 1, q.y1 * ih - 1, layer, r, g, b, a, q.s1, q.t1);
	}
}
