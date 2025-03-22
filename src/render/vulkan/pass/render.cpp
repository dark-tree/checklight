
#include "render.hpp"
#include "render/vulkan/setup/debug.hpp"

/*
 * RenderPass
 */

RenderPass::RenderPass(VkDevice vk_device, VkRenderPass vk_pass, std::vector<VkClearValue>& values, std::vector<Subpass>& subpass_info, FramebufferSet& framebuffer, VkSampleCountFlagBits samples)
: framebuffer(framebuffer), vk_device(vk_device), vk_pass(vk_pass), values(values), subpasses(subpass_info), samples(samples) {
	values.shrink_to_fit();
}

void RenderPass::close() {
	framebuffer.close();
	vkDestroyRenderPass(vk_device, vk_pass, nullptr);
}

const Subpass& RenderPass::getSubpass(int subpass) const {
	return subpasses[subpass];
}

int RenderPass::getSubpassCount() const {
	return subpasses.size();
}

void RenderPass::prepareFramebuffers(const Swapchain& swapchain) {
	framebuffer.construct(vk_pass, vk_device, swapchain);
}

VkFramebuffer RenderPass::getFramebuffer(uint32_t i) {
	return framebuffer.byPresentationIndex(i).getHandle();
}

/*
 * RenderPassBuilder
 */

void RenderPassBuilder::setSampleCount(VkSampleCountFlagBits samples) {
	if (samples == VK_SAMPLE_COUNT_1_BIT) {
		return;
	}

	if (count != VK_SAMPLE_COUNT_1_BIT && count != samples) {
		throw std::runtime_error {"Consistent sample count must be used within a render pass!"};
	}

	this->count = samples;
}

Attachment::Ref RenderPassBuilder::addAttachment(AttachmentBuilder& builder) {
	attachments.push_back(builder);
	framebuffer.addAttachment(builder.attachment);
	return Attachment::Ref::of(attachments.size() - 1);
}

AttachmentBuilder RenderPassBuilder::addAttachment(const Attachment& attachment) {
	values.push_back(attachment.getClearValue());
	setSampleCount(attachment.getSamples());
	return {*this, attachment};
}

RenderPassBuilder& RenderPassBuilder::addSubpass(SubpassBuilder& builder) {
	subpasses.push_back(builder);
	return *this;
}

SubpassBuilder RenderPassBuilder::addSubpass(VkPipelineBindPoint bind_point) {
	preserve.push();
	return {*this, bind_point, (uint32_t) attachments.size(), preserve};
}

RenderPassBuilder& RenderPassBuilder::addDependency(DependencyBuilder& builder) {
	dependencies.push_back(builder);
	return *this;
}

DependencyBuilder RenderPassBuilder::addDependency(VkDependencyFlags flags) {
	return {*this, flags};
}

RenderPass RenderPassBuilder::build(const LogicalDevice& device, const char* name) {

	std::vector<VkAttachmentDescription> attachment_descriptions;
	std::vector<VkSubpassDescription> subpass_descriptions;
	std::vector<VkSubpassDependency> dependency_descriptions;
	std::vector<std::vector<uint32_t>> preserve_indices;
	std::vector<Subpass> subpass_info;

	auto view = preserve.view();

	attachment_descriptions.reserve(attachments.size());
	subpass_descriptions.reserve(subpasses.size());
	dependency_descriptions.reserve(dependencies.size());

	for (auto& attachment : attachments) {
		attachment_descriptions.push_back(attachment.finalize());
	}

	for (auto& dependency : dependencies) {
		dependency_descriptions.push_back(dependency.finalize());
	}

	for (auto& subpass : subpasses) {
		view.up();
		auto set = view.collect();

		preserve_indices.emplace_back();

		for (uint32_t attachment : set) {
			if (subpass.shouldPreserve(attachment)) {
				preserve_indices.back().push_back(attachment);
			}
		}

		subpass_descriptions.push_back(subpass.finalize(preserve_indices.back(), subpass_info));
	}

	VkRenderPassCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	create_info.attachmentCount = (uint32_t) attachment_descriptions.size();
	create_info.pAttachments = attachment_descriptions.data();
	create_info.subpassCount = (uint32_t) subpass_descriptions.size();
	create_info.pSubpasses = subpass_descriptions.data();
	create_info.dependencyCount = (uint32_t) dependency_descriptions.size();
	create_info.pDependencies = dependency_descriptions.data();

	VkRenderPass render_pass;

	if (vkCreateRenderPass(device.getHandle(), &create_info, nullptr, &render_pass) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create render pass!"};
	}

	VulkanDebug::setDebugName(device.getHandle(), VK_OBJECT_TYPE_RENDER_PASS, render_pass, name);
	framebuffer.setDebugName(name);

	return {device.getHandle(), render_pass, values, subpass_info, framebuffer, count};

}