
#include "push.hpp"

/*
 * PushConstant
 */

PushConstant::PushConstant()
: range({}) {}

PushConstant::PushConstant(VkShaderStageFlags stages, uint32_t bytes) {
	range.size = bytes;
	range.offset = 0;
	range.stageFlags = stages;

	if (bytes == 0) {
		throw std::runtime_error {"Push constant must not be empty!"};
	}

	if (bytes % 4 != 0) {
		throw std::runtime_error {"Push constant must be aligned to 4 bytes, but " + std::to_string(bytes) + " was requested!"};
	}
}

const VkPushConstantRange& PushConstant::getRange() const {
	return range;
}