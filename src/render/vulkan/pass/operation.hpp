#pragma once

#include "external.hpp"

struct AttachmentOpType {

	struct Load {
		const VkAttachmentLoadOp vk_load;

		explicit Load(VkAttachmentLoadOp vk_load);
	};

	struct Store {
		const VkAttachmentStoreOp vk_store;

		explicit Store(VkAttachmentStoreOp vk_store);
	};

	struct Both {
		const VkAttachmentLoadOp vk_load;
		const VkAttachmentStoreOp vk_store;

		explicit Both(VkAttachmentLoadOp vk_load, VkAttachmentStoreOp vk_store);
	};

};

template <typename T, typename S>
struct AttachmentOp {

	private:

		const S values;

	public:

		template <typename ...Args>
		constexpr explicit AttachmentOp(Args... args)
		: values(args...) {}

		template <typename R = VkAttachmentLoadOp> requires requires {values.vk_load;}
		constexpr auto load() const -> R {
			return values.vk_load;
		}

		template <typename R = VkAttachmentStoreOp> requires requires {values.vk_store;}
		constexpr auto store() const -> R {
			return values.vk_store;
		}

};

struct ColorOp {

	/// Clear color data at the start of the pass
	static AttachmentOp<ColorOp, AttachmentOpType::Load> CLEAR;

	/// Load color data at the start of the pass
	static AttachmentOp<ColorOp, AttachmentOpType::Load> LOAD;

	/// Store color data at the end of the pass
	static AttachmentOp<ColorOp, AttachmentOpType::Store> STORE;

	/// Leave color data in a undefined, unusable, state
	static AttachmentOp<ColorOp, AttachmentOpType::Both> IGNORE;

};

struct StencilOp {

	/// Clear stencil data at the start of the pass
	static AttachmentOp<StencilOp, AttachmentOpType::Load> CLEAR;

	/// Load stencil data at the start of the pass
	static AttachmentOp<StencilOp, AttachmentOpType::Load> LOAD;

	/// Store stencil data at the end of the pass
	static AttachmentOp<StencilOp, AttachmentOpType::Store> STORE;

	/// Leave stencil data in a undefined, unusable, state
	static AttachmentOp<StencilOp, AttachmentOpType::Both> IGNORE;

};