
#include "mesh.hpp"

RenderMesh::~RenderMesh() {
	vertex.close();
}

Buffer& RenderMesh::getVertexBuffer() {
	return vertex;
}

size_t RenderMesh::getVertexCount() {
	return vertex_count;
}