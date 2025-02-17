#pragma once

#include "external.hpp"
#include "api/vertex.hpp"
#include "api/reusable.hpp"

class CommandRecorder;

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

		VertexChannel basic {"Textured 2D"};

		float iw, ih;
		uint8_t r, g, b, a;
		float layer;

		void upload(CommandRecorder& recorder);
		void close();

	private:

		/// Draw 2D vertex
		void vertex(float x, float y, float u, float v);

		/// Draw 3D vertex
		void vertex(float x, float y, float z, float u, float v);

	public:

		ImmediateRenderer();

		void setLayer(uint32_t layer);
		void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void setResolution(uint32_t width, uint32_t height);

		void clear();
		void drawRect2D(float x, float y, float w, float h);



};
