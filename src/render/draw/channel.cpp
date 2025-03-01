
#include "channel.hpp"
#include <render/vulkan/command/recorder.hpp>

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
	commands.clear();
}

void VertexChannel::pushTransform(glm::mat4 matrix) {
	if (!commands.empty()) {
		Command& command = commands.back();

		if (command.count == 0) {
			command.constant.matrix = matrix;
			return;
		}
	}

	commands.emplace_back(MeshConstant {matrix}, 0);
}

void VertexChannel::draw(PushConstant& push, CommandRecorder& recorder) {
	if (!buffer.isEmpty()) {
		int offset = 0;

		for (const auto& command : commands) {
			recorder.writePushConstant(push, &command.constant).bindVertexBuffer(buffer.getBuffer()).draw(command.count, 1, offset, 0);
			offset += command.count;
		}
	}
}