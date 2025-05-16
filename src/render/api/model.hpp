#pragma once

#include "render/vulkan/raytrace/struct.hpp"
#include "render/api/mesh.hpp"

/**
 * Represents a ready-to-render model of a object in the world, and can
 * be instantiated by referencing it from a RenderObject. RenderModel can be
 * created from a set of meshes using the AccelStructFactory.
 */
class RenderModel {

	private:

		VkDevice vk_device;
		AccelStruct structure;
		VkDeviceAddress address;
		std::shared_ptr<RenderMesh> mesh;

	public:

		RenderModel(const LogicalDevice& device, const AccelStruct& structure);
		~RenderModel();

		/**
		 * Get the underlying address of the stored acceleration structure,
		 * this address is calculated once (during a call to setStructure()) and stored
		 */
		VkDeviceAddress getAddress() const;

		/**
		 * Change the BLAS stored in this model, this is used
		 * after BLAS compaction to point to the new structure.
		 */
		void setStructure(const LogicalDevice& device, const AccelStruct& structure);

		/**
		 * Get the structure stored in this model, usually the structure
		 * stays constant, but it can be change during baking in the AccelStructFactory
		 */
		AccelStruct getStructure() const;

		/**
		 * Set the mesh that was used to create this model
		 */
		void setMesh(const std::shared_ptr<RenderMesh>& mesh);

		/**
		 * Get the mesh that was used to create this model
		 */
		std::shared_ptr<RenderMesh> getMesh() const;

};
