
#include "dependency.hpp"
#include "render.hpp"

/*
 * DependencyBuilder
 */

VkSubpassDependency DependencyBuilder::finalize() {
	return description;
}

DependencyBuilder::DependencyBuilder(RenderPassBuilder& builder, VkDependencyFlags flags)
: builder(builder) {
	description.dependencyFlags = flags;
}

DependencyBuilder& DependencyBuilder::first(uint32_t subpass, VkPipelineStageFlags stage, VkAccessFlags access) {
	description.srcSubpass = subpass;
	description.srcStageMask = stage;
	description.srcAccessMask = access;
	return *this;
}

DependencyBuilder& DependencyBuilder::then(uint32_t subpass, VkPipelineStageFlags stage, VkAccessFlags access) {
	description.dstSubpass = subpass;
	description.dstStageMask = stage;
	description.dstAccessMask = access;
	return *this;
}

RenderPassBuilder& DependencyBuilder::next() {
	return builder.addDependency(*this);
}