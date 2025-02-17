
#include "object.hpp"
#include "shared/math.hpp"
#include "model.hpp"

/*
 * RenderObject
 */

RenderObject::RenderObject(uint32_t index)
: index(index), instance({}) {
	instance.instanceCustomIndex = index;
	instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;

	setActive(true);
	setShader(0);
	setMatrix(glm::identity<glm::mat4x3>());
}

const VkAccelerationStructureInstanceKHR* RenderObject::getInstanceData() const {
	return &instance;
}

uint32_t RenderObject::getIndex() const {
	return index;
}

void RenderObject::setMatrix(const glm::mat4x3& model) {
	instance.transform = math::toVulkanAffine(model);
}

void RenderObject::setShader(uint32_t offset) {
	instance.instanceShaderBindingTableRecordOffset = offset;
}

void RenderObject::setTraits(VkGeometryInstanceFlagsKHR flags) {
	instance.flags = flags;
}

void RenderObject::setModel(const std::shared_ptr<RenderModel>& model, const LogicalDevice& device) {
	this->model = model;
	instance.accelerationStructureReference = model->getAddress();
	
	this->data.vertex_address = device.getAddress(model->getMesh()->getVertexData().getBuffer());
	this->data.index_address = device.getAddress(model->getMesh()->getIndexData().getBuffer());
}

void RenderObject::setActive(bool active) {
	instance.mask = (active ? 0xff : 0x00);
}

const RenderObjectData& RenderObject::getObjectData() const {
	return data;
}
