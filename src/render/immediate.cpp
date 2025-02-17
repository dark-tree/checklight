
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

void ImmediateRenderer::vertex(float x, float y, float u, float v) {
	vertex(x * iw - 1, y * ih - 1, layer, u, v);
}

void ImmediateRenderer::vertex(float x, float y, float z, float u, float v) {
	basic.write(x, y, z, r, g, b, a); // TODO texture UV
}

ImmediateRenderer::ImmediateRenderer() {
	setLayer(0);
	setColor(255, 255, 255);
	setResolution(1, 1);
}

void ImmediateRenderer::setLayer(uint32_t layer) {
	this->layer = 1.0f / (layer + 1);
}

void ImmediateRenderer::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

void ImmediateRenderer::setResolution(uint32_t width, uint32_t height) {
	this->iw = 2.0f / std::max(1u, width);
	this->ih = 2.0f / std::max(1u, height);
}

void ImmediateRenderer::drawRect2D(float x, float y, float w, float h) {
	vertex(x, y, 0.0f, 0.0f);
	vertex(x + w, y, 1.0f, 0.0f);
	vertex(x + w, y + h, 0.0f, 1.0f);

	vertex(x, y, 0.0f, 0.0f);
	vertex(x + w, y + h, 0.0f, 0.0f);
	vertex(x, y + h, 0.0f, 1.0f);
}


