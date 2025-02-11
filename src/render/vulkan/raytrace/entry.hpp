#pragma once

#include "external.hpp"

class LogicalDevice;
class RenderMesh;
class ReusableBuffer;

/**
 * Represents a single acceleration structure geometry entry - that is
 * a single mesh or instance buffer. Multiple entries come together in a
 * AccelerationStructureBuilder
 */
class TraceStructEntry {

	private:

		VkAccelerationStructureBuildRangeInfoKHR range;
		VkAccelerationStructureGeometryKHR geometry;

		TraceStructEntry(VkAccelerationStructureBuildRangeInfoKHR range, VkAccelerationStructureGeometryKHR geometry);

	public:

		static TraceStructEntry ofTriangles(LogicalDevice& device, const RenderMesh& mesh, bool opaque);
		static TraceStructEntry ofInstances(LogicalDevice& device, const ReusableBuffer& buffer, bool opaque);

		const VkAccelerationStructureBuildRangeInfoKHR& getRange() const;
		const VkAccelerationStructureGeometryKHR& getGeometry() const;

};


