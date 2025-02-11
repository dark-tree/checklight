
#include "entry.hpp"
#include "render/vulkan/buffer/buffer.hpp"
#include "render/api/vertex.hpp"
#include "render/api/mesh.hpp"

/*
 * TraceStructEntry
 */

TraceStructEntry::TraceStructEntry(VkAccelerationStructureBuildRangeInfoKHR range, VkAccelerationStructureGeometryKHR geometry)
: range(range), geometry(geometry) {}

TraceStructEntry TraceStructEntry::ofTriangles(LogicalDevice& device, const RenderMesh& mesh, bool opaque) {

	VkAccelerationStructureGeometryKHR geometry {};
	geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometry.pNext = nullptr;
	geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	geometry.flags = (opaque ? VK_GEOMETRY_OPAQUE_BIT_KHR : 0);

	VkAccelerationStructureGeometryTrianglesDataKHR& triangles = geometry.geometry.triangles;
	triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	triangles.pNext = nullptr;
	triangles.transformData = {};

	if (mesh.hasIndexData()) {
		triangles.indexData.deviceAddress = device.getAddress(mesh.getIndexData().getBuffer());
		triangles.indexType = Index3D::format;
	}

	triangles.vertexData.deviceAddress = device.getAddress(mesh.getVertexData().getBuffer());
	triangles.vertexFormat = Vertex3D::position;
	triangles.vertexStride = sizeof(Vertex3D);
	triangles.maxVertex = mesh.getVertexData().getCount() - 1;

	VkAccelerationStructureBuildRangeInfoKHR range {};
	range.firstVertex = 0;
	range.primitiveCount = mesh.getCount() / 3;
	range.primitiveOffset = 0;
	range.transformOffset = 0;

	return {range, geometry};

}

TraceStructEntry TraceStructEntry::ofInstances(LogicalDevice& device, const ReusableBuffer& buffer, bool opaque) {

	VkAccelerationStructureGeometryKHR geometry {};
	geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometry.pNext = nullptr;
	geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	geometry.flags = (opaque ? VK_GEOMETRY_OPAQUE_BIT_KHR : 0);

	VkAccelerationStructureGeometryInstancesDataKHR& instances = geometry.geometry.instances;
	instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	instances.pNext = nullptr;
	instances.arrayOfPointers = false;
	instances.data.deviceAddress = device.getAddress(buffer.getBuffer());

	VkAccelerationStructureBuildRangeInfoKHR range {};
	range.firstVertex = 0;
	range.primitiveCount = buffer.getCount();
	range.primitiveOffset = 0;
	range.transformOffset = 0;

	return {range, geometry};

}

const VkAccelerationStructureBuildRangeInfoKHR& TraceStructEntry::getRange() const {
	return range;
}

const VkAccelerationStructureGeometryKHR& TraceStructEntry::getGeometry() const {
	return geometry;
}