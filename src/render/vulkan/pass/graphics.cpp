
#include "graphics.hpp"

/*
 * GraphicsPipelineBuilder
 */

void GraphicsPipelineBuilder::finalize() {

	// explicit super call
	AbstractPipelineBuilder::finalize();

	input.vertexBindingDescriptionCount = (uint32_t) bindings.size();
	input.pVertexBindingDescriptions = bindings.data();
	input.vertexAttributeDescriptionCount = (uint32_t) attributes.size();
	input.pVertexAttributeDescriptions = attributes.data();

	view.viewportCount = 1;
	view.pViewports = &vk_viewport;
	view.scissorCount = 1;
	view.pScissors = &vk_scissor;

	// technically blending is per-pipeline and per-attachment but we limit it to per-pipeline only here
	for (int i = 0; i < (int) blending.attachmentCount; i ++) {
		attachments.push_back(attachment);
	}

	blending.pAttachments = attachments.data();

}

GraphicsPipelineBuilder::GraphicsPipelineBuilder(const LogicalDevice& device)
: AbstractPipelineBuilder<GraphicsPipelineBuilder>(device) {

	depth.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	view.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	withPrimitive(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	withDepthClamp(false);
	withDepthBias(false);
	withPolygonMode(VK_POLYGON_MODE_FILL);
	withLineWidth(1.0f);
	withCulling(false);

	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.flags = 0;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blending.attachmentCount = 0;
	withBlendConstants(0.0f, 0.0f, 0.0f, 0.0f);
	withBlendWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	withBlendColorFunc(VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);
	withBlendAlphaFunc(VK_BLEND_FACTOR_ONE, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ZERO);
	withBlendBitwiseFunc(VK_LOGIC_OP_COPY);
	withBlendMode(BlendMode::DISABLED);

}

GraphicsPipelineBuilder GraphicsPipelineBuilder::of(const LogicalDevice& device) {
	return {device};
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withViewport(int x, int y, uint32_t width, uint32_t height, float min_depth, float max_depth) {
	vk_viewport.x = (float) x;
	vk_viewport.y = (float) y;
	vk_viewport.width = (float) width;
	vk_viewport.height = (float) height;
	vk_viewport.minDepth = min_depth;
	vk_viewport.maxDepth = max_depth;
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withScissors(int x, int y, uint32_t width, uint32_t height) {
	vk_scissor.offset = {x, y};
	vk_scissor.extent = {width, height};
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withBindingLayout(BindingLayout& layout) {
	const auto binding = (uint32_t) bindings.size();

	// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkVertexInputBindingDescription.html
	VkVertexInputBindingDescription description {};
	description.binding = binding;
	description.stride = layout.getStride();
	description.inputRate = layout.getRate();

	bindings.push_back(description);

	for (VkVertexInputAttributeDescription& attribute : layout.getAttributes()) {
		attribute.binding = binding;
		this->attributes.push_back(attribute);
	}

	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withPrimitive(VkPrimitiveTopology topology, bool enable_restart) {
	assembly.topology = topology;
	assembly.primitiveRestartEnable = enable_restart;
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withDepthClamp(bool enable) {
	rasterizer.depthClampEnable = enable;
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withDepthBias(bool enable, float constant, float clamp, float slope) {
	rasterizer.depthBiasEnable = enable;
	rasterizer.depthBiasConstantFactor = constant;
	rasterizer.depthBiasClamp = clamp;
	rasterizer.depthBiasSlopeFactor = slope;
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withPolygonMode(VkPolygonMode mode) {
	rasterizer.polygonMode = mode;
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withLineWidth(float width) {
	rasterizer.lineWidth = width;
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withCulling(bool enable, VkFrontFace face, VkCullModeFlags mode) {
	rasterizer.cullMode = enable ? mode : VK_CULL_MODE_NONE;
	rasterizer.frontFace = face;
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withDepthBound(float lower, float upper) {
	depth.depthBoundsTestEnable = true;
	depth.minDepthBounds = lower;
	depth.maxDepthBounds = upper;
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withDepthTest(VkCompareOp function, bool read, bool write) {
	depth.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth.depthCompareOp = function;
	depth.depthTestEnable = read;
	depth.depthWriteEnable = write;

	if (write && !read) {
		throw std::runtime_error {"Read must be enabled to use write, use VK_COMPARE_OP_ALWAYS to skip depth tests instead!"};
	}

	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withBlendConstants(float r, float g, float b, float a) {
	blending.blendConstants[0] = r;
	blending.blendConstants[1] = g;
	blending.blendConstants[2] = b;
	blending.blendConstants[3] = a;
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withBlendWriteMask(VkColorComponentFlags flags) {
	attachment.colorWriteMask = flags;
	return *this;

}

// finalColor.rgb = (src * newColor.rgb) <op> (dst * oldColor.rgb);
GraphicsPipelineBuilder& GraphicsPipelineBuilder::withBlendColorFunc(VkBlendFactor src, VkBlendOp op, VkBlendFactor dst) {
	attachment.srcColorBlendFactor = src;
	attachment.dstColorBlendFactor = dst;
	attachment.colorBlendOp = op;
	return *this;
}

// finalColor.a = (<src> * newColor.a) <op> (<dst> * oldColor.a);
GraphicsPipelineBuilder& GraphicsPipelineBuilder::withBlendAlphaFunc(VkBlendFactor src, VkBlendOp op, VkBlendFactor dst) {
	attachment.srcAlphaBlendFactor = src;
	attachment.dstAlphaBlendFactor = dst;
	attachment.colorBlendOp = op;
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withBlendBitwiseFunc(VkLogicOp op) {
	blending.logicOp = op;
	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withBlendMode(BlendMode mode) {
	if (mode == BlendMode::DISABLED) {
		blending.logicOpEnable = false;
		attachment.blendEnable = false;
	}

	if (mode == BlendMode::ENABLED) {
		blending.logicOpEnable = false;
		attachment.blendEnable = true;
	}

	if (mode == BlendMode::BITWISE) {
		blending.logicOpEnable = true;
		attachment.blendEnable = false;
	}

	return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::withRenderPass(RenderPass& render_pass, int subpass_index) {
	const int count = render_pass.getSubpassCount();

	if (count <= subpass_index) {
		FAULT("Specified render pass has ", count, " subpasses but, subpass with index ", subpass_index, " was requested!");
	}

	blending.attachmentCount = render_pass.getSubpass(subpass_index).getAttachmentCount();
	vk_pass = render_pass.vk_pass;
	subpass = subpass_index;
	multisampling.rasterizationSamples = render_pass.samples;
	return *this;
}

GraphicsPipeline GraphicsPipelineBuilder::build() {

	if (subpass == -1) {
		throw std::runtime_error {"Render pass needs to be specified!"};
	}

	finalize();
	VkPipelineLayout pipeline_layout = createPipelineLayout();

	std::vector<VkPipelineShaderStageCreateInfo> shaders;

	for (Shader& stage : stages) {
		shaders.push_back(stage.getConfig());
	}

	VkGraphicsPipelineCreateInfo create_info {};
	create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	create_info.layout = pipeline_layout;

	create_info.stageCount = (uint32_t) shaders.size();
	create_info.pStages = shaders.data();
	create_info.pVertexInputState = &input;
	create_info.pInputAssemblyState = &assembly;
	create_info.pViewportState = &view;
	create_info.pRasterizationState = &rasterizer;
	create_info.pMultisampleState = &multisampling;
	create_info.pDepthStencilState = &depth;
	create_info.pColorBlendState = &blending;
	create_info.pDynamicState = &dynamic;

	create_info.renderPass = vk_pass;
	create_info.subpass = subpass;

	// optional inheritance
	create_info.basePipelineHandle = VK_NULL_HANDLE;
	create_info.basePipelineIndex = -1;

	// init noop stencil
	depth.stencilTestEnable = false;
	depth.front = {}; // Optional
	depth.back = {}; // Optional

	VkPipeline pipeline;

	if (vkCreateGraphicsPipelines(vk_device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline) != VK_SUCCESS) {
		throw std::runtime_error {"Failed to create graphics pipeline!"};
	}

	return {PipelineType::RASTER, pipeline, pipeline_layout, vk_device};

}