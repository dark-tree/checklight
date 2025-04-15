#pragma once

#include "external.hpp"
#include "api/vertex.hpp"
#include "api/reusable.hpp"
#include "asset/atlas.hpp"
#include "asset/font.hpp"
#include "asset/sprite.hpp"
#include "draw/channel.hpp"
#include "draw/color.hpp"
#include "draw/text.hpp"
#include "draw/arc.hpp"
#include "draw/alignment.hpp"

class CommandRecorder;
class AssetLoader;

enum struct BillboardMode {
	ONE_AXIS,
	TWO_AXIS,
};

enum Disabled {
	OFF,
	DISABLED,
};

class ImmediateRenderer {

	private:

		friend class Renderer;

		AssetLoader& loader;
		Sprite blank;

		VertexChannel basic {"Textured 2D"};
		VertexChannel text {"Text 2D"};
		VertexChannel basic_3d {"Textured 3D"};

		float iw, ih;
		Color active;
		Color fill;
		Color stroke;
		float rtl, rtr, rbl, rbr;
		float width;
		Sprite sprite;
		float tx, ty, tw, th;
		int mapping = 0;
		float stroke_width = 0;

		bool stroke_enabled = false;
		bool fill_enabled = true;

		TextBakery bakery;
		ArcQuality quality;
		BillboardMode billboard;
		glm::vec3 target;

		void upload(CommandRecorder& recorder);
		void close(const LogicalDevice& device);
		Texture& getAtlasTexture();
		float getMaxPixelError() const;
		void drawBillboardVertex(glm::quat rotation, glm::vec3 offset, float x, float y, float u, float v);
		glm::quat getBillboardRotation(glm::vec3 center) const;
		void useColor(const Color& color);

	private:

		/// Draw 2D vertex with implicit (mapping based) texture UV
		void drawVertex2D(const Color& color, float x, float y);

		/// Draw 2D vertex with explicit texture UV
		void drawVertex2D(const Color& color, float x, float y, float u, float v);

		/// Draw 2D vertex with explicit texture UV
		void drawVertex2D(const Color& color, glm::vec2 pos, float u, float v);

		/// Get the point on a Bézier curve at the given T value
		float getBezierPoint(float a, float b, float c, float d, float t);

		/// Get the tangent to the Bézier curve at the given T value
		float getBezierTangent(float a, float b, float c, float d, float t);

		/// Changes the window-space texture mapping of the active sprite
		void pushTextureMap(float x, float y, float w, float h);

		/// Pop the applied texture map
		void popTextureMap();

		/// Draws a quad of the given color
		void drawColoredQuad2D(const Color& color, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

	public:

		ImmediateRenderer(AssetLoader& loader);

		void clear();
		Sprite getSprite(const std::string& path);

		void setFill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void setFill(const Color& color);
		void setFill(Disabled disabled);
		void setStroke(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void setStroke(const Color& color);
		void setStroke(Disabled disabled);
		void setResolution(uint32_t width, uint32_t height);
		void setLineWidth(float width);
		void setRectRadius(float radius);
		void setRectRadius(float top, float bottom);
		void setRectRadius(float top_left, float top_right, float bottom_left, float bottom_right);
		void setSprite(Sprite sprite);
		void setSprite(const std::string& path);
		void setSprite(Disabled disable);
		void setFont(const std::string& path);
		void setFont(const std::string& path, int size);
		void setFontSize(int size);
		void setQuality(ArcQuality quality);
		void setMatrix2D(const glm::mat4& matrix);
		void setMatrix3D(const glm::mat4& matrix);
		void setBillboardTarget(glm::vec3 pos);
		void setBillboardMode(BillboardMode mode);
		void setTextAlignment(VerticalAlignment alignment);
		void setTextAlignment(HorizontalAlignment alignment);
		void setTextAlignment(VerticalAlignment vertical, HorizontalAlignment horizontal);
		void setTextBox(Disabled disabled);
		void setTextBox(int width, int height);
		void setWrapping(bool wrap);
		void setStrokeWidth(float stroke);

		// Helpers
		BakedText bakeString(float x, float y, const std::string& text);
		BakedText bakeUnicode(float x, float y, const std::vector<uint32_t>& unicodes);

		// 2D Primitives
		void drawRect2D(float x, float y, float w, float h);
		void drawRect2D(const Box2D& box);
		void drawTrig2D(float x1, float y1, float x2, float y2, float x3, float y3);
		void drawArc2D(float x, float y, float hrad, float vrad, float start, float angle, ArcMode mode = ArcMode::OPEN_PIE);
		void drawLine2D(float x1, float y1, float x2, float y2);
		void drawSlantedLine2D(glm::vec2 p1, glm::vec2 d1, glm::vec2 p2, glm::vec2 d2);
		void drawEllipse2D(float x, float y, float hrad, float vrad);
		void drawCircle2D(float x, float y, float radius);
		void drawQuad2D(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
		void drawBezier2D(float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy);
		void drawString2D(float x, float y, const std::string& str);
		void drawText2D(float x, float y, const BakedText& text);

		// 3D Primitives
		void drawVertex3D(float x, float y, float z, float u, float v);
		void drawVertex3D(glm::vec3 pos, float u, float v);
		void drawLine3D(float x1, float y1, float z1, float x2, float y2, float z2);
		void drawRect3D(float x, float y, float z, float w, float h);

};
