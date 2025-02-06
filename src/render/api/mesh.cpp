
#include "mesh.hpp"

/*
 * RenderMesh
 */

RenderMesh::~RenderMesh() {
	vertex.close();
	index.close();
}

void RenderMesh::setDebugName(const std::string& name) {
	vertex.setDebugName("Vertex " + name);
	index.setDebugName("Index " + name);
}

void RenderMesh::uploadVertices(RenderCommander& commander, const std::vector<Vertex3D>& vertices) {
	vertex.upload(commander, vertices.data(), vertices.size(), sizeof(Vertex3D));
}

void RenderMesh::uploadIndices(RenderCommander& commander, const std::vector<Index3D::type>& indices) {
	index.upload(commander, indices.data(), indices.size(), sizeof(Index3D::type));
}

bool RenderMesh::hasIndexData() const {
	return !index.isEmpty();
}

void RenderMesh::draw(CommandRecorder& recorder) {
	recorder.bindVertexBuffer(vertex.getBuffer());

	if (hasIndexData()) {
		recorder.bindIndexBuffer(index.getBuffer());
		recorder.drawIndexed(index.getCount());
		return;
	}

	// if no index data just draw the vertices
	recorder.draw(vertex.getCount());
}

size_t RenderMesh::getCount() const {
	return hasIndexData() ? index.getCount() : vertex.getCount();
}

const ReusableBuffer& RenderMesh::getVertexData() const {
	return vertex;
}

const ReusableBuffer& RenderMesh::getIndexData() const {
	return index;
}