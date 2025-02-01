
#include "attachment.hpp"
#include "render.hpp"

/*
 * AttachmentBuilder
 */

VkAttachmentDescription AttachmentBuilder::finalize() {
	return description;
}

AttachmentBuilder& AttachmentBuilder::input(VkAttachmentLoadOp color, VkAttachmentLoadOp stencil, VkImageLayout layout) {
	description.loadOp = color;
	description.stencilLoadOp = stencil;
	description.initialLayout = layout;
	return *this;
}

AttachmentBuilder& AttachmentBuilder::output(VkAttachmentStoreOp color, VkAttachmentStoreOp stencil, VkImageLayout layout) {
	description.storeOp = color;
	description.stencilStoreOp = stencil;
	description.finalLayout = layout;
	return *this;
}

AttachmentBuilder::AttachmentBuilder(RenderPassBuilder& builder, const Attachment& attachment, VkSampleCountFlagBits samples)
: attachment(attachment), builder(builder) {
	description.format = attachment.getFormat();
	description.samples = samples;
}

Attachment::Ref AttachmentBuilder::next() {
	return builder.addAttachment(*this);
}