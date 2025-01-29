#pragma once

#include "external.hpp"

class PushConstant {

	private:

		VkPushConstantRange range;

	public:

		PushConstant();
		PushConstant(VkShaderStageFlags stages, uint32_t bytes);

		const VkPushConstantRange& getRange() const;

};
