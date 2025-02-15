#pragma once

#include "external.hpp"
#include "commander.hpp"
#include "reusable.hpp"
#include "vertex.hpp"

/**
 * Represents a indexed (or not indexed) triangle mesh,
 * can be converted into a model at the AccelStructFactory.
 */
class RenderMesh {

	private:

		ReusableBuffer vertex {VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR};
		ReusableBuffer index {VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR};

	public:

		~RenderMesh();

		/// Set resource name
		void setDebugName(const std::string& name);

		/// Upload vertex data to the underlying buffer
		void uploadVertices(RenderCommander& commander, const std::vector<Vertex3D>& vertices);

		/// Upload index data to the underlying buffer
		void uploadIndices(RenderCommander& commander, const std::vector<Index3D::type>& indices);

		/// Check if index data was uploaded ot this mesh
		bool hasIndexData() const;

		/// Get final number of elements in the mesh
		size_t getCount() const;

		/// Get underlying vertex buffer
		const ReusableBuffer& getVertexData() const;

		/// Get underlying index buffer
		const ReusableBuffer& getIndexData() const;

};
