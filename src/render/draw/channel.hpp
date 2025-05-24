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
		int offset;
		int count;
		bool sync = false;
	};

	using element = Vertex3D;

	int command_index = 0;
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

	/// Push a synchronization point
	void pushSync();

	/// Issue the 'bind' and 'draw' commands
	bool draw(PushConstant& push, CommandRecorder& recorder);

	/// Write one element into the local buffer
	template <typename... Args>
	void write(Args... values) {
		commands.back().count ++;
		vertices.emplace_back(values...);
	}

};
