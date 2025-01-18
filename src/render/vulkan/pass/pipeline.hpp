#pragma once

#include "external.hpp"
#include "render/vulkan/shader.hpp"
#include "render/vulkan/binding.hpp"
#include "render/vulkan/descriptor/layout.hpp"
#include "render/vulkan/pass/render.hpp"

enum struct BlendMode {
	BITWISE = 1,
	ENABLED = 2,
	DISABLED = 3
};

class GraphicsPipeline {

	private:

		VkPipeline vk_pipeline;
		VkPipelineLayout vk_layout;
		VkDevice vk_device;

	public:

		GraphicsPipeline() = default;
		GraphicsPipeline(VkPipeline vk_pipeline, VkPipelineLayout vk_layout, VkDevice vk_device);

		void close();

};

class GraphicsPipelineBuilder {

	private:

		std::vector<VkDescriptorSetLayout> sets;
		std::vector<VkDynamicState> dynamics;
		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;
		std::vector<VkPipelineColorBlendAttachmentState> attachments;

		std::vector<Shader> stages;

		VkPipelineViewportStateCreateInfo view {};
		VkPipelineDynamicStateCreateInfo dynamic {};
		VkPipelineVertexInputStateCreateInfo input {};
		VkPipelineLayoutCreateInfo layout {};
		VkPipelineInputAssemblyStateCreateInfo assembly {};
		VkPipelineRasterizationStateCreateInfo rasterizer {};
		VkPipelineMultisampleStateCreateInfo multisampling {};
		VkPipelineColorBlendAttachmentState attachment {};
		VkPipelineColorBlendStateCreateInfo blending {};
		VkPipelineDepthStencilStateCreateInfo depth {};

		VkViewport vk_viewport {};
		VkRect2D vk_scissor {};
		VkRenderPass vk_pass;
		int subpass = -1;
		VkDevice vk_device;

		void finalize();

	public:

		GraphicsPipelineBuilder(LogicalDevice device);
		static GraphicsPipelineBuilder of(LogicalDevice device);

		// dynamic configuration

		template <typename... DynamicState>
		GraphicsPipelineBuilder& withDynamics(DynamicState... states) {
			dynamics = { states... };
			return *this;
		}

		// view configuration

		GraphicsPipelineBuilder& withViewport(int x, int y, uint32_t width, uint32_t height, float min_depth = 0.0f, float max_depth = 1.0f);
		GraphicsPipelineBuilder& withScissors(int x, int y, uint32_t width, uint32_t height);

		// vertex configuration

		GraphicsPipelineBuilder& withBindingLayout(BindingLayout& layout);

		// layout configuration

		GraphicsPipelineBuilder& withDescriptorSetLayout(const DescriptorSetLayout& layout);

		// assembly configuration

		GraphicsPipelineBuilder& withPrimitive(VkPrimitiveTopology topology, bool enable_restart = false);

		// rasterizer configuration

		GraphicsPipelineBuilder& withDepthClamp(bool enable);
		GraphicsPipelineBuilder& withDepthBias(bool enable, float constant = 0.0f, float clamp = 0.0f, float slope = 0.0f);
		GraphicsPipelineBuilder& withPolygonMode(VkPolygonMode mode);
		GraphicsPipelineBuilder& withLineWidth(float width);
		GraphicsPipelineBuilder& withCulling(bool enable, VkFrontFace face = VK_FRONT_FACE_CLOCKWISE, VkCullModeFlags mode = VK_CULL_MODE_BACK_BIT);

		// depth configuration

		GraphicsPipelineBuilder& withDepthBound(float lower, float upper);
		GraphicsPipelineBuilder& withDepthTest(VkCompareOp function, bool read, bool write);

		// blend configuration

		GraphicsPipelineBuilder& withBlendConstants(float r, float g, float b, float a);
		// finalColor = finalColor & writeMask;
		GraphicsPipelineBuilder& withBlendWriteMask(VkColorComponentFlags flags);
		// finalColor.rgb = (src * newColor.rgb) <op> (dst * oldColor.rgb);
		GraphicsPipelineBuilder& withBlendColorFunc(VkBlendFactor src, VkBlendOp op, VkBlendFactor dst);
		// finalColor.a = (<src> * newColor.a) <op> (<dst> * oldColor.a);
		GraphicsPipelineBuilder& withBlendAlphaFunc(VkBlendFactor src, VkBlendOp op, VkBlendFactor dst);
		GraphicsPipelineBuilder& withBlendBitwiseFunc(VkLogicOp op);
		GraphicsPipelineBuilder& withBlendMode(BlendMode mode);

		// renderpass configuration

		GraphicsPipelineBuilder& withRenderPass(RenderPass& render_pass, int subpass_index);

		// shader stage configuration

		template<typename... Shaders>
		GraphicsPipelineBuilder& withShaders(Shaders... shaders) {
			stages = { shaders... };
			return *this;
		}

	public:

		GraphicsPipeline build();

};
