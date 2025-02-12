
#include "model.hpp"

/*
 * RenderModel
 */

RenderModel::RenderModel(const LogicalDevice& device, AccelStruct structure)
: structure(structure), address(device.getAddress(structure)) {}

VkDeviceAddress RenderModel::getAddress() const {
	return address;
}