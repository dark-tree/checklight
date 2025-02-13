
#include "raytrace.hpp"

/*
 * RaytracePipelineBuilder
 */

RaytracePipelineBuilder::RaytracePipelineBuilder(const LogicalDevice& device)
: AbstractPipelineBuilder<RaytracePipelineBuilder>(device) {
	this->limit = device.getPhysical()->getMaxRaytraceRecursionDepth();
}

RaytracePipelineBuilder RaytracePipelineBuilder::of(const LogicalDevice& device) {
	return {device};
}

RaytracePipelineBuilder& RaytracePipelineBuilder::withRecursionDepth(int depth) {
	if (depth > limit) {
		throw std::runtime_error {"Requested recursion depth of " + std::to_string(depth) + " is bigger that the supported value of " + std::to_string(limit)};
	}

	this->depth = depth;
	return *this;
}