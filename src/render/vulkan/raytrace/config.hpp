#pragma once

#include "external.hpp"
#include "entry.hpp"
#include "struct.hpp"

class LogicalDevice;
class RenderMesh;
class ReusableBuffer;
class AccelStructBakedConfig;
class RenderModel;

/**
 * Represents the configuration of a single acceleration
 * structure, feed this config to a AccelStructFactory to
 * create the corresponding AccelStruct. Config can only be used (baked) once!
 */
class AccelStructConfig {

	private:

		VkAccelerationStructureKHR vk_struct = VK_NULL_HANDLE;
		VkBuildAccelerationStructureModeKHR vk_mode;
		VkAccelerationStructureTypeKHR vk_type;
		VkBuildAccelerationStructureFlagsKHR vk_flags;

		std::vector<VkAccelerationStructureBuildRangeInfoKHR> ranges;
		std::vector<VkAccelerationStructureGeometryKHR> geometries;

		void addEntry(const AccelStructEntry& entry);

	public:

		enum Operation {
			BUILD = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
			UPDATE = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR,
		};

		enum Type {
			TOP = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
			BOTTOM = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
		};

		static AccelStructConfig create(Operation operation, Type type);

	public:

		/// Add triangle geometry buffer to the config, used by BLASes
		AccelStructConfig& addTriangles(LogicalDevice& device, const RenderMesh& mesh, bool opaque);

		/// Add instance buffer to the config, used by TLASes
		AccelStructConfig& addInstances(LogicalDevice& device, const ReusableBuffer& instances, bool opaque);

		/// Set the operation that should be performed during baking in the AccelStructFactory
		AccelStructConfig& setOperation(Operation operation, Type type);

		/// Set additional flags
		AccelStructConfig& setFlags(VkBuildAccelerationStructureFlagsKHR flags);

		/// In some cases (e.g. performing an UPDATE) an additional, parental AccelStruct can be supplied
		AccelStructConfig& setParent(const AccelStruct& structure);

		/// This method should not be used, it's used internally by AccelStructFactory
		AccelStructBakedConfig bake(const LogicalDevice& device, Allocator& allocator);

};

/**
 * Intermediate baked version of the config with
 * semi-prepared vulkan structures, used internally
 * by the AccelStructFactory
 */
struct AccelStructBakedConfig {

	std::shared_ptr<RenderModel> model;
	VkAccelerationStructureBuildGeometryInfoKHR build_info;
	VkAccelerationStructureBuildSizesInfoKHR size_info;

	std::vector<VkAccelerationStructureBuildRangeInfoKHR> ranges;
	std::vector<VkAccelerationStructureGeometryKHR> geometries;

	bool ready = false;

	uint32_t getScratchSize() const;
	void setScratch(VkDeviceAddress scratch);

};