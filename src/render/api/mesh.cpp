
#include "mesh.hpp"

RenderMesh::~RenderMesh() {
	vertex.close();
	index.close();
}

bool RenderMesh::hasIndexData() const {
	return index_count != 0;
}

void RenderMesh::draw(CommandRecorder& recorder) {
	recorder.bindVertexBuffer(vertex);

	if (hasIndexData()) {
		recorder.bindIndexBuffer(index);
		recorder.drawIndexed(index_count);
		return;
	}

	recorder.draw(vertex_count);
}
