
#include "operation.hpp"

/*
 * AttachmentOpType
 */

AttachmentOpType::Load::Load::Load(VkAttachmentLoadOp vk_load)
: vk_load(vk_load) {}

AttachmentOpType::Store::Store(VkAttachmentStoreOp vk_store)
: vk_store(vk_store) {}

AttachmentOpType::Both::Both(VkAttachmentLoadOp vk_load, VkAttachmentStoreOp vk_store)
: vk_load(vk_load), vk_store(vk_store) {}

/*
 * ColorOp
 */

AttachmentOp<ColorOp, AttachmentOpType::Load> ColorOp::CLEAR {VK_ATTACHMENT_LOAD_OP_CLEAR};
AttachmentOp<ColorOp, AttachmentOpType::Load> ColorOp::LOAD {VK_ATTACHMENT_LOAD_OP_LOAD};
AttachmentOp<ColorOp, AttachmentOpType::Store> ColorOp::STORE {VK_ATTACHMENT_STORE_OP_STORE};
AttachmentOp<ColorOp, AttachmentOpType::Both> ColorOp::IGNORE {VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE};

/*
 * StencilOp
 */

AttachmentOp<StencilOp, AttachmentOpType::Load> StencilOp::CLEAR {VK_ATTACHMENT_LOAD_OP_CLEAR};
AttachmentOp<StencilOp, AttachmentOpType::Load> StencilOp::LOAD {VK_ATTACHMENT_LOAD_OP_LOAD};
AttachmentOp<StencilOp, AttachmentOpType::Store> StencilOp::STORE {VK_ATTACHMENT_STORE_OP_STORE};
AttachmentOp<StencilOp, AttachmentOpType::Both> StencilOp::IGNORE {VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE};
