#pragma once

#include "external.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/api/mesh.hpp"

class AccelerationStructure {

	private:

		Buffer buffer;
		VkAccelerationStructureKHR vk_structure;

	public:

		AccelerationStructure(const Buffer& buffer, VkAccelerationStructureKHR vk_structure);

};

class AccelerationStructureEntry {

	private:

		VkAccelerationStructureBuildRangeInfoKHR range;
		VkAccelerationStructureGeometryKHR geometry;

		AccelerationStructureEntry(VkAccelerationStructureBuildRangeInfoKHR range, VkAccelerationStructureGeometryKHR geometry);

	public:

		static AccelerationStructureEntry ofTriangles(LogicalDevice& device, const RenderMesh& mesh, bool opaque);
		static AccelerationStructureEntry ofInstances(LogicalDevice& device, const ReusableBuffer& buffer, bool opaque);

};