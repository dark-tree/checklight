#pragma once

#include "external.hpp"
#include "entry.hpp"
#include "struct.hpp"

class LogicalDevice;
class RenderMesh;
class ReusableBuffer;
class TraceStructBakedConfig;

class TraceStructConfig {

	private:

		VkAccelerationStructureKHR vk_struct = VK_NULL_HANDLE;
		VkBuildAccelerationStructureModeKHR vk_mode;
		VkAccelerationStructureTypeKHR vk_type;

		std::vector<VkAccelerationStructureBuildRangeInfoKHR> ranges;
		std::vector<VkAccelerationStructureGeometryKHR> geometries;

		void addEntry(const TraceStructEntry& entry);

	public:

		enum Operation {
			BUILD = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
			UPDATE = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR,
		};

		enum Type {
			TOP = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
			BOTTOM = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
		};

	public:

		TraceStructConfig& addTriangles(LogicalDevice& device, const RenderMesh& mesh, bool opaque);
		TraceStructConfig& addInstances(LogicalDevice& device, const ReusableBuffer& instances, bool opaque);

		TraceStructConfig& setOperation(Operation operation, Type type);
		TraceStructConfig& setParent(const TraceStruct& structure);

		TraceStructBakedConfig bake(const LogicalDevice& device) const;

};

struct TraceStructBakedConfig {

	VkAccelerationStructureBuildGeometryInfoKHR build_info;
	VkAccelerationStructureBuildSizesInfoKHR size_info;
	std::vector<VkAccelerationStructureBuildRangeInfoKHR> ranges;

	uint32_t getScratchSize() const;

};