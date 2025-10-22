
#include "object.hpp"
#include "shared/math.hpp"
#include "model.hpp"

/*
 * RenderObject
 */

RenderObject::RenderObject(uint32_t index)
: index(index), instance({}), raster_instance({}) {
	instance.instanceCustomIndex = index;
	instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;

	setActive(true);
	setShader(0);
	setMatrix(glm::identity<glm::mat4x3>());
	setPortal(glm::zero<glm::mat4>());
}

const VkAccelerationStructureInstanceKHR* RenderObject::getInstanceData() const {
	return &instance;
}

const RasterInstanceData* RenderObject::getRasterInstanceData() const {
	return &raster_instance;
}

const RenderObjectData* RenderObject::getObjectData() const {
	return &data;
}

uint32_t RenderObject::getIndex() const {
	return index;
}

void RenderObject::setMatrix(const glm::mat4x3& model) {
	instance.transform = math::toVulkanAffine(model);
	raster_instance.model = glm::mat4(model);
}

void RenderObject::setShader(uint32_t offset) {
	instance.instanceShaderBindingTableRecordOffset = offset;
}

void RenderObject::setTraits(VkGeometryInstanceFlagsKHR flags) {
	instance.flags = flags;
}

void RenderObject::setModel(const std::shared_ptr<RenderModel>& model) {
	this->model = model;
	system.addMeshModel(model);
	instance.accelerationStructureReference = model->getAddress();
	data.vertex_address = model->getMesh()->getVertexData().getDeviceAddress();
	data.index_address = model->getMesh()->getIndexData().getDeviceAddress();
}

std::shared_ptr<RenderModel> RenderObject::getModel() {
	return model;
}

void RenderObject::setActive(bool active) {
	instance.mask = (active ? 0xff : 0x00);
}

void RenderObject::setPortal(const glm::mat4& portal) {
	data.portal = portal;
}

void RenderObject::setIndex(uint32_t index) {
	this->index = index;
}
