#pragma once

#include "external.hpp"
#include "commander.hpp"
#include "reusable.hpp"

class RenderMesh {

	private:

		ReusableBuffer vertex {VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};
		ReusableBuffer index {VK_BUFFER_USAGE_INDEX_BUFFER_BIT};

	public:

		~RenderMesh();

		template <typename V>
		void uploadVertices(RenderCommander& commander, const std::vector<V>& vertices) {
			vertex.upload(commander, vertices.data(), vertices.size(), sizeof(V));
		}

		template <typename I>
		void uploadIndices(RenderCommander& commander, const std::vector<I>& indices) {
			index.upload(commander, indices.data(), indices.size(), sizeof(I));
		}

		bool hasIndexData() const;
		void draw(CommandRecorder& recorder);


};
