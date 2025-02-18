#pragma once

#include "external.hpp"
#include "api/vertex.hpp"
#include "api/reusable.hpp"
#include "render/asset/atlas.hpp"

class CommandRecorder;

enum ArcMode {
	OPEN_PIE,
	OPEN_CHORD
};

/**
 * Easy to use vertex upload system
 */
struct VertexChannel {
	using element = Vertex3D;

	std::vector<element> vertices;
	ReusableBuffer buffer {VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};

	VertexChannel(const std::string& string);

	/// Free underlying vulkan objects
	void close();

	/// Update data held in GPU buffers
	void upload(CommandRecorder& recorder);

	/// Check if this channel has any data
	bool empty() const;

	/// Clear local data, doesn't affect the GPU buffers
	void clear();

	/// Write one element into the local buffer
	template <typename... Args>
	void write(Args... values) {
		vertices.emplace_back(values...);
	}

};

class ImmediateRenderer {

	private:

		friend class Renderer;

		std::shared_ptr<DynamicAtlas> atlas;
		DynamicImageAtlas images;

		VertexChannel basic {"Textured 2D"};

		float iw, ih;
		uint8_t r, g, b, a;
		float rtl, rtr, rbl, rbr;
		float layer, width;
		Sprite sprite;

		void upload(CommandRecorder& recorder);
		void close();

	private:

		/// Draw 2D vertex
		void vertex(float x, float y, float u, float v);

		/// Draw 2D vertex
		void vertex(glm::vec2 pos, float u, float v);

		/// Draw 3D vertex
		void vertex(float x, float y, float z, float u, float v);

		/// Get the point on a bezier curve at the given T value
		float getBezierPoint(float a, float b, float c, float d, float t);

		/// Get the tangent to the bezier curve at the given T value
		float getBezierTangent(float a, float b, float c, float d, float t);

	public:

		ImmediateRenderer();

		void clear();

		Sprite getSprite(const std::string& path);

		void setLayer(uint32_t layer);
		void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void setResolution(uint32_t width, uint32_t height);
		void setLineWidth(float width);
		void setRectRadius(float radius);
		void setRectRadius(float top, float bottom);
		void setRectRadius(float top_left, float top_right, float bottom_left, float bottom_right);
		void setSprite(Sprite sprite);

		// 2D Primitives
		void drawRect2D(float x, float y, float w, float h);
		void drawArc2D(float x, float y, float hrad, float vrad, float start, float angle, ArcMode mode = OPEN_PIE);
		void drawLine2D(float x1, float y1, float x2, float y2);
		void drawSlantedLine2D(glm::vec2 p1, glm::vec2 d1, glm::vec2 p2, glm::vec2 d2);
		void drawEllipse2D(float x, float y, float hrad, float vrad);
		void drawCircle2D(float x, float y, float radius);
		void drawQuad2D(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
		void drawTrig2D(float x1, float y1, float x2, float y2, float x3, float y3);
		void drawBezier2D(float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy);

};
