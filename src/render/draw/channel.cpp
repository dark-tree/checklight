
#include "channel.hpp"
#include <render/vulkan/command/recorder.hpp>

/*
 * VertexChannel
 */

VertexChannel::VertexChannel(const std::string& string) {
	buffer.setDebugName("Immediate " + string + " Vertex Channel");
	clear();
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
	command_index = 0;
	vertices.clear();
	commands.clear();

	commands.emplace_back(MeshConstant {glm::identity<glm::mat4>()}, 0, 0);
}

void VertexChannel::pushTransform(glm::mat4 matrix) {
	Command& command = commands.back();

	if (command.count == 0) {
		command.constant.matrix = matrix;
	} else {
		commands.emplace_back(MeshConstant {glm::identity<glm::mat4>()}, vertices.size(), 0);
	}
}

void VertexChannel::pushSync() {
	commands.back().sync = true;
	commands.emplace_back(MeshConstant {glm::identity<glm::mat4>()}, vertices.size(), 0);
}

bool VertexChannel::draw(PushConstant& push, CommandRecorder& recorder) {
	if (!buffer.isEmpty()) {
		for (; command_index < (int) commands.size(); command_index ++) {
			Command& command = commands[command_index];

			if (command.count != 0) {
				recorder.writePushConstant(push, &command.constant).bindVertexBuffer(buffer.getBuffer()).draw(command.count, 1, command.offset, 0);
			}

			if (command.sync) {
				// only tell the renderer to retry if we actually have something to do
				return static_cast<size_t>(++ command_index) != commands.size();
			}
		}
	}

	return false;
}