
#include "config.hpp"
#include "render/vulkan/setup/proxy.hpp"

/*
 * TraceStructConfig
 */

void TraceStructConfig::addEntry(const TraceStructEntry& entry) {
	ranges.emplace_back(entry.getRange());
	geometries.emplace_back(entry.getGeometry());
}

TraceStructConfig& TraceStructConfig::addTriangles(LogicalDevice& device, const RenderMesh& mesh, bool opaque) {
	addEntry(TraceStructEntry::ofTriangles(device, mesh, opaque));
	return *this;
}

TraceStructConfig& TraceStructConfig::addInstances(LogicalDevice& device, const ReusableBuffer& instances, bool opaque) {
	addEntry(TraceStructEntry::ofInstances(device, instances, opaque));
	return *this;
}

TraceStructConfig& TraceStructConfig::setOperation(Operation operation, Type type) {
	this->vk_mode = static_cast<VkBuildAccelerationStructureModeKHR>(operation);
	this->vk_type = static_cast<VkAccelerationStructureTypeKHR>(type);
	return *this;
}

TraceStructConfig& TraceStructConfig::setParent(const TraceStruct& structure) {
	this->vk_struct = structure.getHandle();
	return *this;
}

TraceStructBakedConfig TraceStructConfig::bake(const LogicalDevice& device) const {
	VkAccelerationStructureBuildGeometryInfoKHR build_info {};
	build_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	build_info.pNext = nullptr;
	build_info.type = vk_type;
	build_info.mode = vk_mode;
	build_info.srcAccelerationStructure = vk_struct;
	build_info.geometryCount = geometries.size();
	build_info.pGeometries = geometries.data();

	std::vector<uint32_t> primitives;
	primitives.reserve(ranges.size());

	for (const auto& range : ranges) {
		primitives.push_back(range.primitiveCount);
	}

	VkAccelerationStructureBuildSizesInfoKHR size_info;
	Proxy::vkGetAccelerationStructureBuildSizesKHR(device.getHandle(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &build_info, primitives.data(), &size_info);

	return {build_info, size_info, ranges};
}

/*
 * TraceStructBakedConfig
 */

uint32_t TraceStructBakedConfig::getScratchSize() const {
	if (build_info.mode == VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR) {
		return size_info.buildScratchSize;
	}

	return size_info.updateScratchSize;
}
