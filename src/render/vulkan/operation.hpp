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
	static AttachmentOp<ColorOp, AttachmentOpType::Load> CLEAR;
	static AttachmentOp<ColorOp, AttachmentOpType::Load> LOAD;
	static AttachmentOp<ColorOp, AttachmentOpType::Store> STORE;
	static AttachmentOp<ColorOp, AttachmentOpType::Both> IGNORE;
};

struct StencilOp {
	static AttachmentOp<StencilOp, AttachmentOpType::Load> CLEAR;
	static AttachmentOp<StencilOp, AttachmentOpType::Load> LOAD;
	static AttachmentOp<StencilOp, AttachmentOpType::Store> STORE;
	static AttachmentOp<StencilOp, AttachmentOpType::Both> IGNORE;
};