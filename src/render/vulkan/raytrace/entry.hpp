#pragma once

#include "external.hpp"

class LogicalDevice;
class RenderMesh;
class ReusableBuffer;

/**
 * Represents a single acceleration structure geometry entry - that is
 * a single mesh or instance buffer. Multiple entries come together in a
 * AccelStructConfig to form a single BLAS or TLAS
 */
class AccelStructEntry {

	private:

		VkAccelerationStructureBuildRangeInfoKHR range;
		VkAccelerationStructureGeometryKHR geometry;

		AccelStructEntry(VkAccelerationStructureBuildRangeInfoKHR range, VkAccelerationStructureGeometryKHR geometry);

	public:

		/// Create an entry of a triangle geometry buffer, used by BLASes
		static AccelStructEntry ofTriangles(LogicalDevice& device, const RenderMesh& mesh, bool opaque);

		/// Create an entry of an instance buffer, used by TLASes
		static AccelStructEntry ofInstances(LogicalDevice& device, uint32_t instanceCount, const ReusableBuffer& buffer, bool opaque);

		/// Get the vulkan range info of this entry
		const VkAccelerationStructureBuildRangeInfoKHR& getRange() const;

		/// Get the vulkan geometry info of this entry
		const VkAccelerationStructureGeometryKHR& getGeometry() const;

};


