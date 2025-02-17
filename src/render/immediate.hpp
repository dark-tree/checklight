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

		void upload(CommandRecorder& recorder);
		void close();

	public:

		void clear();
		void drawRect2D(float x, float y, float w, float h);



};
