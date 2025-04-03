#pragma once

#include "external.hpp"
#include "render/vulkan/setup/device.hpp"

class RenderModel;

struct RenderObjectData {
	uint64_t vertex_address;
	uint64_t index_address;
	glm::mat4 portal;
};

/**
 * Represents a instance of a RenderModel that is placed in the world
 * at a specific position dictated by the given affine transform matrix
 */
class RenderObject {

	private:

		std::shared_ptr<RenderModel> model;
		uint32_t index;
		VkAccelerationStructureInstanceKHR instance;
		RenderObjectData data;

		void setShader(uint32_t index);

	public:

		RenderObject(uint32_t index);

		const VkAccelerationStructureInstanceKHR* getInstanceData() const;
		const RenderObjectData* getObjectData() const;
		uint32_t getIndex() const;

		/**
		 * Update the affine transform matrix of this render object,
		 * this allows the object to be moved, rotated, scaled, stretched
		 * but no perspective transform can be applied. Initially objects
		 * are at the world origin using identity model transform.
		 *
		 * @param model affine transform matrix
		 */
		void setMatrix(const glm::mat4x3& model);

		/**
		 * Set object specific instance traits, this allows to
		 * control opacity and polygon facing.
		 *
		 * @param flags set of vulkan instance flags
		 */
		void setTraits(VkGeometryInstanceFlagsKHR flags);

		/**
		 * Set the model this instance will use - it's geometry.
		 * You can obtain a model object from the asset loader.
		 *
		 * @param model object model to use for this instance
		 * @param device logical device to use for address retrieval
		 */
		void setModel(const std::shared_ptr<RenderModel>& model);

		/**
		 * Controls whether the object is visible (Active) or not
		 * (Inactive), allows for easy hiding of object without removing,
		 * initially objects are in the active state.
		 *
		 * @param active true for active, false otherwise
		 */
		void setActive(bool active);

		/**
		 * Set the transformation of rays that pass through this object
		 * 
		 * @param portal transformation matrix
		 */
		void setPortal(const glm::mat4& portal);

};