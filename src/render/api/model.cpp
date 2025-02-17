
#include "model.hpp"

/*
 * RenderModel
 */

RenderModel::RenderModel(const LogicalDevice& device, const AccelStruct& structure) {
	setStructure(device, structure);
}

VkDeviceAddress RenderModel::getAddress() const {
	return address;
}

void RenderModel::setStructure(const LogicalDevice& device, const AccelStruct& structure) {
	this->structure = structure;
	this->address = device.getAddress(structure);
}

AccelStruct RenderModel::getStructure() const {
	return structure;
}

void RenderModel::setMesh(const std::shared_ptr<RenderMesh>& mesh) {
	this->mesh = mesh;
}

std::shared_ptr<RenderMesh> RenderModel::getMesh() const {
	return mesh;
}
