#pragma once

#include "external.hpp"
#include "api/vertex.hpp"
#include "api/reusable.hpp"
#include "asset/atlas.hpp"
#include "asset/font.hpp"
#include "asset/sprite.hpp"

class CommandRecorder;

enum struct ArcMode {
	OPEN_PIE,
	OPEN_CHORD
};

enum struct ArcQuality {
	VERY_LOW   = 100,
	LOW        =  75,
	MEDIUM     =  45,
	HIGH       =  33,
	VERY_HIGH  =  25,
};

enum struct VerticalAlignment {
	TOP    = 2,
	CENTER = 1,
	BOTTOM = 0
};

enum struct HorizontalAlignment {
	LEFT   = 0,
	CENTER = 1,
	RIGHT  = 2
};

enum struct BillboardMode {
	ONE_AXIS,
	TWO_AXIS,
};

enum Disabled {
	OFF,
	DISABLED,
};

struct MeshConstant {
	glm::mat4 matrix;
};


/**
 * Easy to use vertex upload system
 */
struct VertexChannel {

	struct Command {
		MeshConstant constant;
		int count;
	};

	using element = Vertex3D;

	std::vector<element> vertices;
	std::vector<Command> commands;
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

	/// Make future write with this matrix
	void pushTransform(glm::mat4 matrix);

	/// Issue the 'bind' and 'draw' commands
	void draw(PushConstant& push, CommandRecorder& recorder);

	/// Write one element into the local buffer
	template <typename... Args>
	void write(Args... values) {
		if (commands.empty()) {
			pushTransform(glm::identity<glm::mat4>());
		}

		commands.back().count ++;
		vertices.emplace_back(values...);
	}

};

class ImmediateRenderer {

	private:

		friend class Renderer;

		Sprite blank;
		std::shared_ptr<DynamicAtlas> atlas;
		DynamicImageAtlas images;
		DynamicFontAtlas fonts;
		std::shared_ptr<Font> font;

		VertexChannel basic {"Textured 2D"};
		VertexChannel text {"Text 2D"};
		VertexChannel basic_3d {"Textured 3D"};

		float iw, ih;
		uint8_t r, g, b, a;
		float rtl, rtr, rbl, rbr;
		float width;
		Sprite sprite;
		float tx, ty, tw, th;
		int mapping = 0;
		float font_size;
		float font_tilt;
		ArcQuality quality;
		BillboardMode billboard;
		glm::vec3 target;
		VerticalAlignment vertical;
		HorizontalAlignment horizontal;

		void upload(CommandRecorder& recorder);
		void close();
		Texture& getAtlasTexture();
		float getMaxPixelError() const;
		void drawBillboardVertex(glm::quat rotation, glm::vec3 offset, float x, float y, float u, float v);
		glm::quat getBillboardRotation(glm::vec3 center) const;
		glm::vec2 getTextOffset(const std::vector<uint32_t>& text, glm::vec2 extend) const;

	private:

		/// Draw 2D vertex with implicit (mapping based) texture UV
		void drawVertex2D(float x, float y);

		/// Get the point on a bezier curve at the given T value
		float getBezierPoint(float a, float b, float c, float d, float t);

		/// Get the tangent to the bezier curve at the given T value
		float getBezierTangent(float a, float b, float c, float d, float t);

		/// Changes the window-space texture mapping of the active sprite
		void pushTextureMap(float x, float y, float w, float h);

		/// Pop the applied texture map
		void popTextureMap();

	public:

		ImmediateRenderer();

		void clear();
		Sprite getSprite(const std::string& path);

		void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
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
		void setFontTilt(float tilt);
		void setTextAlignment(VerticalAlignment alignment);
		void setTextAlignment(HorizontalAlignment alignment);
		void setTextAlignment(VerticalAlignment vertical, HorizontalAlignment horizontal);

		// 2D Primitives
		void drawVertex2D(float x, float y, float u, float v);
		void drawVertex2D(glm::vec2 pos, float u, float v);
		void drawRect2D(float x, float y, float w, float h);
		void drawArc2D(float x, float y, float hrad, float vrad, float start, float angle, ArcMode mode = ArcMode::OPEN_PIE);
		void drawLine2D(float x1, float y1, float x2, float y2);
		void drawSlantedLine2D(glm::vec2 p1, glm::vec2 d1, glm::vec2 p2, glm::vec2 d2);
		void drawEllipse2D(float x, float y, float hrad, float vrad);
		void drawCircle2D(float x, float y, float radius);
		void drawQuad2D(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
		void drawBezier2D(float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy);
		void drawText2D(float x, float y, const std::string& str);

		// 3D Primitives
		void drawVertex3D(float x, float y, float z, float u, float v);
		void drawVertex3D(glm::vec3 pos, float u, float v);
		void drawLine3D(float x1, float y1, float z1, float x2, float y2, float z2);
		void drawRect3D(float x, float y, float z, float w, float h);

};
