#pragma once

#include "external.hpp"
#include "commander.hpp"
#include "reusable.hpp"
#include "vertex.hpp"

class RenderMesh {

	private:

		ReusableBuffer vertex {VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR};
		ReusableBuffer index {VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR};

	public:

		~RenderMesh();

		void setDebugName(const std::string& name);

		void uploadVertices(RenderCommander& commander, const std::vector<Vertex3D>& vertices);
		void uploadIndices(RenderCommander& commander, const std::vector<Index3D::type>& indices);

		bool hasIndexData() const;
		void draw(CommandRecorder& recorder);
		size_t getCount() const;

		const ReusableBuffer& getVertexData() const;
		const ReusableBuffer& getIndexData() const;

};
