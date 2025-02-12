
#include "config.hpp"
#include "render/vulkan/setup/proxy.hpp"
#include "render/api/model.hpp"

/*
 * AccelStructConfig
 */

void AccelStructConfig::addEntry(const AccelStructEntry& entry) {
	ranges.emplace_back(entry.getRange());
	geometries.emplace_back(entry.getGeometry());
}

AccelStructConfig AccelStructConfig::create(Operation operation, Type type) {
	AccelStructConfig config {};
	config.setOperation(operation, type);
	return config;
}

AccelStructConfig& AccelStructConfig::addTriangles(LogicalDevice& device, const RenderMesh& mesh, bool opaque) {
	addEntry(AccelStructEntry::ofTriangles(device, mesh, opaque));
	return *this;
}

AccelStructConfig& AccelStructConfig::addInstances(LogicalDevice& device, const ReusableBuffer& instances, bool opaque) {
	addEntry(AccelStructEntry::ofInstances(device, instances, opaque));
	return *this;
}

AccelStructConfig& AccelStructConfig::setOperation(Operation operation, Type type) {
	this->vk_mode = static_cast<VkBuildAccelerationStructureModeKHR>(operation);
	this->vk_type = static_cast<VkAccelerationStructureTypeKHR>(type);
	return *this;
}

AccelStructConfig& AccelStructConfig::setFlags(VkBuildAccelerationStructureFlagsKHR flags) {
	this->vk_flags = flags;
	return *this;
}

AccelStructConfig& AccelStructConfig::setParent(const AccelStruct& structure) {
	this->vk_struct = structure.getHandle();
	return *this;
}

AccelStructBakedConfig AccelStructConfig::bake(const LogicalDevice& device, Allocator& allocator) {
	VkAccelerationStructureBuildGeometryInfoKHR build_info {};
	build_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	build_info.pNext = nullptr;
	build_info.type = vk_type;
	build_info.mode = vk_mode;
	build_info.flags = vk_flags;
	build_info.srcAccelerationStructure = vk_struct;
	build_info.geometryCount = geometries.size();
	build_info.pGeometries = geometries.data();

	std::vector<uint32_t> primitives;
	primitives.reserve(ranges.size());

	for (const auto& range : ranges) {
		primitives.push_back(range.primitiveCount);
	}

	VkAccelerationStructureBuildSizesInfoKHR size_info;
	size_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	size_info.pNext = nullptr;

	Proxy::vkGetAccelerationStructureBuildSizesKHR(device.getHandle(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &build_info, primitives.data(), &size_info);

	AccelStruct structure = allocator.allocateAcceleration(build_info.type, size_info.accelerationStructureSize, "Unnamed AccelStruct");
	build_info.dstAccelerationStructure = structure.getHandle();

	return {std::make_shared<RenderModel>(device, structure), build_info, size_info, std::move(ranges), std::move(geometries)};
}

/*
 * AccelStructBakedConfig
 */

uint32_t AccelStructBakedConfig::getScratchSize() const {
	if (build_info.mode == VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR) {
		return size_info.buildScratchSize;
	}

	return size_info.updateScratchSize;
}

void AccelStructBakedConfig::setScratch(VkDeviceAddress scratch) {
	build_info.scratchData.deviceAddress = scratch;
	ready = true;
}