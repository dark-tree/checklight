#pragma once

#include "external.hpp"
#include "shared/pyramid.hpp"
#include "render/vulkan/buffer/attachment.hpp"

class RenderPassBuilder;

class Subpass {

	private:

		uint32_t attachments;

	public:

		Subpass() = default;
		Subpass(uint32_t attachments);

		uint32_t getAttachmentCount() const;

};

/**
 * Define a single pass though the rendering pipeline,
 * this later corresponds with pipelines. One subpass - one pipeline.
 */
class SubpassBuilder {

	private:

		RenderPassBuilder& builder;
		VkSubpassDescription description {};
		uint32_t attachment_count;
		Pyramid<uint32_t>& preserve;
		std::set<uint32_t> references;

		std::vector<VkAttachmentReference> inputs;
		std::vector<VkAttachmentReference> colors;
		std::vector<VkAttachmentReference> resolves;
		VkAttachmentReference depth;

		friend class RenderPassBuilder;

		VkAttachmentReference getReference(uint32_t attachment, VkImageLayout layout);
		bool shouldPreserve(uint32_t attachment) const;
		VkSubpassDescription finalize(const std::vector<uint32_t>& preserve, std::vector<Subpass>& subpass_attachments);

	public:

		SubpassBuilder(RenderPassBuilder& builder, VkPipelineBindPoint bind_point, uint32_t attachment_count, Pyramid<uint32_t>& preserve);

		/**
		 * @brief Add attachment for shader input
		 *
		 * The attachment from which the shader can read using 'uniform subpassInput' and
		 * 'subpassLoad()' GLSL constructs, it's importnat to note that only the pixel being process can be read
		 * from the input attachment, if you need to access arbitrary texels use two render passes and a normal texture sampler.
		 *
		 * @param attachment     The attachment reference obtained from RenderPassBuilder::addAttachment().
		 * @param target_layout  The layout the attachment shall assume after this subpass ends.
		 */
		SubpassBuilder& addInput(Attachment::Ref attachment, VkImageLayout target_layout);

		/**
		 * @brief Add attachment for color data
		 *
		 * The attachment into which data will be written by the shader program,
		 * there can be multiple output attachments for one subpass. This is associated with
		 * a GLSL location index using descriptor sets.
		 *
		 * @param attachment     The attachment reference obtained from RenderPassBuilder::addAttachment().
		 * @param target_layout  The layout the attachment shall assume after this subpass ends.
		 */
		SubpassBuilder& addOutput(Attachment::Ref attachment, VkImageLayout target_layout);

		/**
		 * @brief Add attachment for depth and stencil data
		 *
		 * The attachment used as source of depth and stencil data, both as input and output of it,
		 * depending on pipeline configuration. There can only by one depth attachment per Subpass.
		 *
		 * @param attachment     The attachment reference obtained from RenderPassBuilder::addAttachment().
		 * @param target_layout  The layout the attachment shall assume after this subpass ends.
		 */
		SubpassBuilder& addDepth(Attachment::Ref attachment, VkImageLayout target_layout);

		/**
		 * @brief Add attachment for multisampling color attachments
		 *
		 * @param attachment    The attachment reference obtained from RenderPassBuilder::addAttachment().
		 * @param target_layout The layout the attachment shall assume after this subpass ends.
		 */
		SubpassBuilder& addResolve(Attachment::Ref attachment, VkImageLayout target_layout);

		/// Continue building the Render Pass
		RenderPassBuilder& next();

};