
#include "immediate.hpp"
#include "vulkan/command/recorder.hpp"

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
}

/*
 * ImmediateRenderer
 */

void ImmediateRenderer::upload(CommandRecorder& recorder) {
	basic.upload(recorder);

	recorder.memoryBarrier()
		.first(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT)
		.then(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)
		.byRegionBit()
		.done();
}

void ImmediateRenderer::close() {
	basic.close();
}

void ImmediateRenderer::clear() {
	basic.clear();
}

void ImmediateRenderer::drawRect2D(float x, float y, float w, float h) {
	basic.write(x, y, 1.0f, 255, 255, 255, 255);
	basic.write(x + w, y, 1.0f, 255, 255, 255, 255);
	basic.write(x + w, y + h, 1.0f, 255, 255, 255, 255);

	basic.write(x, y, 1.0f, 255, 255, 255, 255);
	basic.write(x + w, y + h, 1.0f, 255, 255, 255, 255);
	basic.write(x, y + h, 1.0f, 255, 255, 255, 255);
}


