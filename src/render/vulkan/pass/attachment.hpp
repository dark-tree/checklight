#pragma once

#include "external.hpp"
#include "operation.hpp"
#include "render/vulkan/buffer/attachment.hpp"

class RenderPassBuilder;

class AttachmentBuilder {

	private:

		const Attachment& attachment;
		RenderPassBuilder& builder;
		VkAttachmentDescription description {};

		friend class RenderPassBuilder;

		VkAttachmentDescription finalize();
		AttachmentBuilder& input(VkAttachmentLoadOp color, VkAttachmentLoadOp stencil, VkImageLayout layout);
		AttachmentBuilder& output(VkAttachmentStoreOp color, VkAttachmentStoreOp stencil, VkImageLayout layout);

	public:

		AttachmentBuilder(RenderPassBuilder& builder, const Attachment& attachment);

		/**
		 * @brief Specify load operation.
		 *
		 * Describes how the attachment data should be treated
		 * on load at the start of a render pass
		 *
		 * @param color describes what should happen to the color data
		 * @param stencil describes what should happen to the stencil data
		 * @param layout describes what layout should be used
		 */
		template <typename C, typename S>
		AttachmentBuilder& begin(AttachmentOp<ColorOp, C> color, AttachmentOp<StencilOp, S> stencil, VkImageLayout layout) {
			return input(color.load(), stencil.load(), layout);
		}

		/**
		 * @brief Specify load operation.
		 *
		 * Describes how the attachment data should be treated
		 * on load at the start of a render pass
		 *
		 * @param color describes what should happen to the color data
		 * @param layout describes what layout should be used
		 */
		template <typename C>
		AttachmentBuilder& begin(AttachmentOp<ColorOp, C> color, VkImageLayout layout) {
			return begin(color, StencilOp::IGNORE, layout);
		}

		/**
		 * @brief Specify store operation.
		 *
		 * Describes how the attachment data should be treated
		 * after the render pass completes
		 *
		 * @param color describes what should happen to the color data
		 * @param stencil describes what should happen to the stencil data
		 * @param layout describes what layout should be used
		 */
		template <typename C, typename S>
		AttachmentBuilder& end(AttachmentOp<ColorOp, C> color, AttachmentOp<StencilOp, S> stencil, VkImageLayout layout) {
			return output(color.store(), stencil.store(), layout);
		}

		/**
		 * @brief Specify store operation.
		 *
		 * Describes how the attachment data should be treated
		 * after the render pass completes
		 *
		 * @param color describes what should happen to the color data
		 * @param layout describes what layout should be used
		 */
		template <typename C>
		AttachmentBuilder& end(AttachmentOp<ColorOp, C> color, VkImageLayout layout) {
			return end(color, StencilOp::IGNORE, layout);
		}

		/// Continue building the Render Pass
		Attachment::Ref next();

};