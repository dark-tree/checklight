
#include "mesh.hpp"

/*
 * RenderMesh
 */

RenderMesh::~RenderMesh() {
	vertex.close();
	index.close();
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
