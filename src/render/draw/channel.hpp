#pragma once

#include <external.hpp>
#include <render/api/vertex.hpp>
#include <render/api/reusable.hpp>

class PushConstant;
class CommandRecorder;

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
