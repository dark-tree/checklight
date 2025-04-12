
#include "immediate.hpp"

#include "system.hpp"
#include "vulkan/command/recorder.hpp"
#include "asset/atlas.hpp"
#include "shared/unicode.hpp"
#include "shared/math.hpp"

/*
 * ImmediateRenderer
 */

void ImmediateRenderer::upload(CommandRecorder& recorder) {
	basic.upload(recorder);
	basic_3d.upload(recorder);
	text.upload(recorder);
	bool dump = loader.getSharedAtlas()->upload(recorder);

	if (mapping != 0) {
		FAULT("Texture mapping stack overflow!");
	}

	if (dump) {
		loader.getSharedAtlas()->getImage().save("debug/atlas.png");
	}

	recorder.memoryBarrier()
		.first(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT)
		.then(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)
		.byRegionBit()
		.done();
}

void ImmediateRenderer::close(const LogicalDevice& device) {
	basic.close();
	text.close();
	basic_3d.close();
	loader.getSharedAtlas()->close(device);
}

Texture& ImmediateRenderer::getAtlasTexture() {
	return loader.getSharedAtlas()->getTexture();
}

void ImmediateRenderer::clear() {
	basic.clear();
	text.clear();
	basic_3d.clear();
}

void ImmediateRenderer::drawVertex2D(const Color& color, float x, float y) {
	if (!mapping) {
		FAULT("No texture mapped!");
	}

	float du = sprite.u2 - sprite.u1;
	float dv = sprite.v2 - sprite.v1;

	float u = sprite.u1 + (x - tx) / tw * du;
	float v = sprite.v1 + (y - ty) / th * dv;

	drawVertex2D(color, x, y, u, v);
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

void ImmediateRenderer::drawColoredQuad2D(const Color& color, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {

	if (mapping) {
		drawVertex2D(color, x1, y1);
		drawVertex2D(color, x2, y2);
		drawVertex2D(color, x3, y3);

		drawVertex2D(color, x1, y1);
		drawVertex2D(color, x3, y3);
		drawVertex2D(color, x4, y4);
		return;
	}

	drawVertex2D(color, x1, y1, sprite.u1, sprite.v1);
	drawVertex2D(color, x2, y2, sprite.u2, sprite.v1);
	drawVertex2D(color, x3, y3, sprite.u2, sprite.v2);

	drawVertex2D(color, x1, y1, sprite.u1, sprite.v1);
	drawVertex2D(color, x3, y3, sprite.u2, sprite.v2);
	drawVertex2D(color, x4, y4, sprite.u1, sprite.v2);

}


float ImmediateRenderer::getMaxPixelError() const {
	return ((float) quality) / 100.0f;
}

void ImmediateRenderer::drawBillboardVertex(glm::quat rotation, glm::vec3 offset, float x, float y, float u, float v) {
	drawVertex3D((rotation * glm::vec3 {-x, -y, 0}) + offset, u, v);
}

glm::quat ImmediateRenderer::getBillboardRotation(glm::vec3 center) const {
	glm::vec3 facing = glm::normalize(target - center);

	const float yaw = math::fastAtan2(facing.x, facing.z);
	glm::quat ry = glm::angleAxis(yaw, glm::vec3(0, 1, 0));

	if (billboard == BillboardMode::TWO_AXIS) {
		const float pitch = math::fastAtan2(-facing.y, glm::length(glm::vec2(facing.x, facing.z)));
		glm::quat rx = glm::angleAxis(pitch, ry * glm::vec3(1, 0, 0));

		return rx * ry;
	}

	return ry;
}

void ImmediateRenderer::useColor(const Color& color) {
	this->active = color;
}

ImmediateRenderer::ImmediateRenderer(AssetLoader& loader)
: loader(loader), blank(loader.getBlankSprite()) {
	setSprite(OFF);
	setFill(255, 255, 255);
	setStroke(255, 0, 0);
	setResolution(1, 1);
	setLineWidth(4);
	setRectRadius(0);
	setFontSize(20);
	setQuality(ArcQuality::HIGH);
	setBillboardMode(BillboardMode::ONE_AXIS);
	setBillboardTarget({0, 0, 0});
	setTextAlignment(VerticalAlignment::BOTTOM);
	setTextAlignment(HorizontalAlignment::LEFT);
	setWrapping(true);
	setStrokeWidth(10);
}

Sprite ImmediateRenderer::getSprite(const std::string& path) {
	return loader.getSprite(path);
}

void ImmediateRenderer::setFill(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	setFill({r, g, b, a});
}

void ImmediateRenderer::setFill(const Color& color) {
	fill_enabled = (color.a != 0);
	this->fill = color;
}

void ImmediateRenderer::setFill(Disabled disabled) {
	fill_enabled = false;
}

void ImmediateRenderer::setStroke(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	setStroke({r, g, b, a});
}

void ImmediateRenderer::setStroke(const Color& color) {
	stroke_enabled = (color.a != 0);
	this->stroke = color;
}

void ImmediateRenderer::setStroke(Disabled disabled) {
	stroke_enabled = false;
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

void ImmediateRenderer::setSprite(Disabled disable) {
	setSprite(this->blank);
}

void ImmediateRenderer::setFont(const std::string& path) {
	bakery.setFont(path);
}

void ImmediateRenderer::setFontSize(int size) {
	bakery.setSize(size);
}

void ImmediateRenderer::setQuality(ArcQuality quality) {
	this->quality = quality;
}

void ImmediateRenderer::setMatrix2D(const glm::mat4& matrix) {
	basic.pushTransform(matrix);
	text.pushTransform(matrix);
}

void ImmediateRenderer::setMatrix3D(const glm::mat4& matrix) {
	basic_3d.pushTransform(matrix);
}

void ImmediateRenderer::setBillboardTarget(glm::vec3 pos) {
	this->target = pos;
}

void ImmediateRenderer::setBillboardMode(BillboardMode mode) {
	this->billboard = mode;
}

void ImmediateRenderer::setTextAlignment(VerticalAlignment alignment) {
	bakery.setAlignment(alignment);
}

void ImmediateRenderer::setTextAlignment(HorizontalAlignment alignment) {
	bakery.setAlignment(alignment);
}

void ImmediateRenderer::setTextAlignment(VerticalAlignment vertical, HorizontalAlignment horizontal) {
	setTextAlignment(vertical);
	setTextAlignment(horizontal);
}

void ImmediateRenderer::setTextBox(Disabled disabled) {
	setTextBox(0, 0);
}

void ImmediateRenderer::setTextBox(int width, int height) {
	bakery.setBounds(width, height);
}

void ImmediateRenderer::setWrapping(bool wrap) {
	bakery.setWrapping(wrap);
}

void ImmediateRenderer::setStrokeWidth(float stroke) {
	this->stroke_width = stroke;
}

BakedText ImmediateRenderer::bakeString(float x, float y, const std::string& text) {
	return bakeUnicode(x, y, utf8::toCodePoints(text.c_str()));
}

BakedText ImmediateRenderer::bakeUnicode(float x, float y, const std::vector<uint32_t>& unicodes) {
	return bakery.bakeUnicode(x, y, unicodes);
}

void ImmediateRenderer::setFont(const std::string& path, int size) {
	setFont(path);
	setFontSize(size);
}

void ImmediateRenderer::drawVertex2D(const Color& color, float x, float y, float u, float v) {
	basic.write(x * iw - 1, y * ih - 1, 0, color.r, color.g, color.b, color.a, u, v);
}

void ImmediateRenderer::drawVertex2D(const Color& color, glm::vec2 pos, float u, float v) {
	drawVertex2D(color, pos.x, pos.y, u, v);
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

	if (fill_enabled) {

		// main rect body
		useColor(fill);
		drawQuad2D(par.x, par.y, pbr.x, pbr.y, pcr.x, pcr.y, pdr.x, pdr.y);

		// beveled walls
		drawQuad2D(par.x, par.y, pdr.x, pdr.y, pdr.x - rtl, pdr.y, par.x - rbl, par.y);
		drawQuad2D(pbr.x, pbr.y, pcr.x, pcr.y, pcr.x + rtr, pcr.y, pbr.x + rbr, pbr.y);
		drawQuad2D(pcr.x, pcr.y, pdr.x, pdr.y, pdr.x, pdr.y - rtl, pcr.x, pcr.y - rtr);
		drawQuad2D(par.x, par.y, pbr.x, pbr.y, pbr.x, pbr.y + rbr, par.x, par.y + rbl);
	}

	if (stroke_enabled) {

		// extended radii
		const float erbl = rbl + stroke_width;
		const float erbr = rbr + stroke_width;
		const float ertr = rtr + stroke_width;
		const float ertl = rtl + stroke_width;

		// beveled stroke
		drawColoredQuad2D(stroke, pdr.x - rtl, pdr.y, par.x - rbl, par.y, par.x - erbl, par.y, pdr.x - ertl, pdr.y);
		drawColoredQuad2D(stroke, pcr.x + rtr, pcr.y, pbr.x + rbr, pbr.y, pbr.x + erbr, pbr.y, pcr.x + ertr, pcr.y);
		drawColoredQuad2D(stroke, pdr.x, pdr.y - rtl, pcr.x, pcr.y - rtr, pcr.x, pcr.y - ertr, pdr.x, pdr.y - ertl);
		drawColoredQuad2D(stroke, pbr.x, pbr.y + rbr, par.x, par.y + rbl, par.x, par.y + erbl, pbr.x, pbr.y + erbr);

	}

	popTextureMap();

}

void ImmediateRenderer::drawRect2D(const Box2D& box) {
	drawRect2D(box.x, box.y, box.w, box.h);
}

void ImmediateRenderer::drawLine2D(float x1, float y1, float x2, float y2) {

	glm::vec2 pa {x1, y1};
	glm::vec2 pb {x2, y2};

	glm::vec2 ab = pb - pa;
	glm::vec2 pp = glm::normalize(glm::vec2 {-ab.y, ab.x}) * width;

	drawVertex2D(fill, pa + pp, sprite.u1, sprite.v1);
	drawVertex2D(fill, pb - pp, sprite.u2, sprite.v2);
	drawVertex2D(fill, pb + pp, sprite.u1, sprite.v2);

	drawVertex2D(fill, pa + pp, sprite.u1, sprite.v1);
	drawVertex2D(fill, pa - pp, sprite.u2, sprite.v1);
	drawVertex2D(fill, pb - pp, sprite.u2, sprite.v2);

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

	drawVertex2D(fill, a1.x, a1.y, sprite.u1, sprite.v1);
	drawVertex2D(fill, a2.x, a2.y, sprite.u2, sprite.v1);
	drawVertex2D(fill, b1.x, b1.y, sprite.u1, sprite.v2);

	drawVertex2D(fill, b1.x, b1.y, sprite.u1, sprite.v2);
	drawVertex2D(fill, a2.x, a2.y, sprite.u2, sprite.v1);
	drawVertex2D(fill, b2.x, b2.y, sprite.u2, sprite.v2);
}

void ImmediateRenderer::drawArc2D(float x, float y, float hrad, float vrad, float start, float angle, ArcMode mode) {

	pushTextureMap(x - hrad, y - vrad, hrad * 2, vrad * 2);

	float herad = hrad + stroke_width;
	float verad = vrad + stroke_width;
	float extent = std::max(herad, verad);

	float theta = 2 * acos(1 - getMaxPixelError() / extent);
	int sides = (int) std::max(2.0f, (abs(angle) / theta));
	float step = angle / sides;

	for (int i = 0; i < sides; i ++) {
		float ax = x + hrad * cos(start + step * i);
		float ay = y + vrad * sin(start + step * i);

		float bx = x + hrad * cos(start + step * (i + 1));
		float by = y + vrad * sin(start + step * (i + 1));

		if (fill_enabled) {
			drawVertex2D(fill, x, y);
			drawVertex2D(fill, ax, ay);
			drawVertex2D(fill, bx, by);
		}

		if (stroke_enabled) {
			float cx = x + herad * cos(start + step * i);
			float cy = y + verad * sin(start + step * i);

			float dx = x + herad * cos(start + step * (i + 1));
			float dy = y + verad * sin(start + step * (i + 1));

			drawVertex2D(stroke, ax, ay);
			drawVertex2D(stroke, cx, cy);
			drawVertex2D(stroke, dx, dy);

			drawVertex2D(stroke, ax, ay);
			drawVertex2D(stroke, dx, dy);
			drawVertex2D(stroke, bx, by);
		}
	}

	if (angle > M_PI && mode == ArcMode::OPEN_CHORD) {
		float ax = x + hrad * cos(start);
		float ay = y + vrad * sin(start);

		float bx = x + hrad * cos(start + angle);
		float by = y + vrad * sin(start + angle);

		if (fill_enabled) {
			drawVertex2D(fill, x, y);
			drawVertex2D(fill, ax, ay);
			drawVertex2D(fill, bx, by);
		}
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
	drawColoredQuad2D(fill, x1, y1, x2, y2, x3, y3, x4, y4);
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
	float spacing = 10.0f + 60.0f * getMaxPixelError() * 3;
	float parts = bound / spacing;
	float segments = parts + (12 / parts + 3);
	float step = 1.0f / segments;

	// The last point is calculated outside the loop
	const float ex = getBezierTangent(ax, bx, cx, dx, 1.0f);
	const float ey = getBezierTangent(ay, by, cy, dy, 1.0f);

	while (t < 1.0f) {

		// Calculate point on the Bézier curve
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

void ImmediateRenderer::drawString2D(float x, float y, const std::string& str) {
	drawText2D(0, 0, bakeString(x, y, str));
}

void ImmediateRenderer::drawText2D(float x, float y, const BakedText& baked) {
	for (GlyphQuad quad : baked.getQuads()) {
		if (quad.shouldDraw()) {

			const float x0 = (quad.x0 + x) * iw - 1;
			const float x1 = (quad.x1 + x) * iw - 1;
			const float y0 = (quad.y0 + y) * ih - 1;
			const float y1 = (quad.y1 + y) * ih - 1;

			text.write(x0, y1, 0, fill.r, fill.g, fill.b, fill.a, quad.s0, quad.t1);
			text.write(x0, y0, 0, fill.r, fill.g, fill.b, fill.a, quad.s0, quad.t0);
			text.write(x1, y0, 0, fill.r, fill.g, fill.b, fill.a, quad.s1, quad.t0);

			text.write(x0, y1, 0, fill.r, fill.g, fill.b, fill.a, quad.s0, quad.t1);
			text.write(x1, y0, 0, fill.r, fill.g, fill.b, fill.a, quad.s1, quad.t0);
			text.write(x1, y1, 0, fill.r, fill.g, fill.b, fill.a, quad.s1, quad.t1);
		}
	}
}

void ImmediateRenderer::drawVertex3D(float x, float y, float z, float u, float v) {
	basic_3d.write(x, y, z, fill.r, fill.g, fill.b, fill.a, u, v);
}

void ImmediateRenderer::drawVertex3D(glm::vec3 pos, float u, float v) {
	drawVertex3D(pos.x, pos.y, pos.z, u, v);
}

void ImmediateRenderer::drawLine3D(float x1, float y1, float z1, float x2, float y2, float z2) {

	glm::vec3 pa {x1, y1, z1};
	glm::vec3 pb {x2, y2, z2};
	glm::vec3 ab = glm::normalize(pb - pa);

	float radius = width / 2;
	int sides = 4;
	float slice = (2 * glm::pi<float>()) / sides;

	for (int i = 0; i < sides; i ++) {
		float ac = (i + 0) * slice + glm::pi<float>() / 4;
		float an = (i + 1) * slice + glm::pi<float>() / 4;

		glm::vec3 vc = math::rotateAlongAxis(ab, ac) * radius;
		glm::vec3 vn = math::rotateAlongAxis(ab, an) * radius;

		//               / Pa + Vc           / Pb + Vc
		//        * --- x -- -- -- -- -- -- x
		//      /  Vc /   \ / Pa + Vn        \  / Pb + Vn
		//     *     /     x -- -- -- -- -- -- x
		//    |     * _.-`  |                  |
		//    |   Pa    Vn  |                  |
		//     *           * -- -- -- -- -- -- *
		//      \         /                   /
		//        * --- * -- -- -- -- -- -- *

		// side
		drawVertex3D(pa + vc, blank.u1, blank.v1);
		drawVertex3D(pb + vn, blank.u2, blank.v2);
		drawVertex3D(pb + vc, blank.u1, blank.v2);

		drawVertex3D(pa + vc, blank.u1, blank.v1);
		drawVertex3D(pa + vn, blank.u2, blank.v1);
		drawVertex3D(pb + vn, blank.u2, blank.v2);

		// caps
		drawVertex3D(pa     , blank.u1, blank.v1);
		drawVertex3D(pa + vn, blank.u2, blank.v1);
		drawVertex3D(pa + vc, blank.u2, blank.v2);

		drawVertex3D(pb     , blank.u1, blank.v1);
		drawVertex3D(pb + vn, blank.u2, blank.v1);
		drawVertex3D(pb + vc, blank.u2, blank.v2);

	}
}

void ImmediateRenderer::drawRect3D(float x, float y, float z, float w, float h) {

	glm::vec3 pos {x, y, z};

	const float sx = -w / 2;
	const float sy = -h / 2;
	const float ex = sx + w;
	const float ey = sy + h;

	glm::quat rot = getBillboardRotation(pos);

	drawBillboardVertex(rot, pos, sx, sy, sprite.u1, sprite.v1);
	drawBillboardVertex(rot, pos, ex, ey, sprite.u2, sprite.v2);
	drawBillboardVertex(rot, pos, sx, ey, sprite.u1, sprite.v2);

	drawBillboardVertex(rot, pos, sx, sy, sprite.u1, sprite.v1);
	drawBillboardVertex(rot, pos, ex, sy, sprite.u2, sprite.v1);
	drawBillboardVertex(rot, pos, ex, ey, sprite.u2, sprite.v2);

}