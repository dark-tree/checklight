
#include "subpass.hpp"
#include "render.hpp"

/*
 * Subpass
 */

Subpass::Subpass(uint32_t attachments)
: attachments(attachments) {}

uint32_t Subpass::getAttachmentCount() const {
	return attachments;
}

/*
 * SubpassBuilder
 */

VkAttachmentReference SubpassBuilder::getReference(uint32_t attachment, VkImageLayout layout) {

	if (attachment >= attachment_count) {
		throw std::runtime_error {"Attachment index " + std::to_string(attachment) + " out of bounds, only " + std::to_string(attachment_count) + " have been defined up to this point!"};
	}

	preserve.append(attachment);

	VkAttachmentReference reference {};
	reference.attachment = attachment;
	reference.layout = layout;

	return reference;

}

bool SubpassBuilder::shouldPreserve(uint32_t attachment) const {
	return !references.contains(attachment);
}

VkSubpassDescription SubpassBuilder::finalize(const std::vector<uint32_t>& preserve, std::vector<Subpass>& subpass_attachments) {

	uint32_t input_count = inputs.size();
	uint32_t color_count = colors.size();
	uint32_t resolve_count = resolves.size();

	if (resolve_count != 0 && resolve_count != color_count) {
		throw std::runtime_error {"Invalid number of resolve attachments! Must be 0 or equal to the number of color attachments!"};
	}

	description.inputAttachmentCount = input_count;
	description.pInputAttachments = inputs.data();
	description.colorAttachmentCount = color_count;
	description.pColorAttachments = colors.data();
	description.pResolveAttachments = resolves.data();
	description.pDepthStencilAttachment = &depth;
	description.preserveAttachmentCount = (uint32_t) preserve.size();
	description.pPreserveAttachments = preserve.data();

	// this is here so that the renderpass can retain the information about
	// how many attachments were there for each subpass - this is then used during pipeline
	// creation to setup blending for each attachment
	subpass_attachments.emplace_back(color_count);

	return description;

}

SubpassBuilder::SubpassBuilder(RenderPassBuilder& builder, VkPipelineBindPoint bind_point, uint32_t attachment_count, Pyramid<uint32_t>& preserve)
: builder(builder), attachment_count(attachment_count), preserve(preserve) {
	description.pipelineBindPoint = bind_point;
	depth.attachment = VK_ATTACHMENT_UNUSED;
	depth.layout = VK_IMAGE_LAYOUT_UNDEFINED;
}

SubpassBuilder& SubpassBuilder::addInput(Attachment::Ref attachment, VkImageLayout target_layout) {
	references.insert(attachment.index);
	inputs.push_back(getReference(attachment.index, target_layout));
	return *this;
}

SubpassBuilder& SubpassBuilder::addOutput(Attachment::Ref attachment, VkImageLayout target_layout) {
	references.insert(attachment.index);
	colors.push_back(getReference(attachment.index, target_layout));
	return *this;
}

SubpassBuilder& SubpassBuilder::addDepth(Attachment::Ref attachment, VkImageLayout target_layout) {
	references.insert(attachment.index);
	depth = getReference(attachment.index, target_layout);
	return *this;
}

SubpassBuilder& SubpassBuilder::addResolve(Attachment::Ref attachment, VkImageLayout target_layout) {
	references.insert(attachment.index);
	resolves.push_back(getReference(attachment.index, target_layout));
	return *this;
}

RenderPassBuilder& SubpassBuilder::next() {
	return builder.addSubpass(*this);
}
