#pragma once

#include "external.hpp"
#include "render.hpp"
#include "render/vulkan/shader/shader.hpp"
#include "render/vulkan/descriptor/vertex.hpp"
#include "render/vulkan/descriptor/layout.hpp"

// concept used for dynamic states
template<typename T>
concept DynamicStateTrait = std::is_same<T, VkDynamicState>::value;

// concept used for shaders states
template<typename T>
concept ShaderTrait = std::is_same<T, Shader>::value;

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

		/// get handle to the underlying piplien object
		VkPipeline getHandle() const;

		/// Get handle to the underlying pipeline layout object
		VkPipelineLayout getLayout() const;

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

		/**
		 * Adds a vulkan dynamic state, that is, a pipeline property that is not static to the
		 * pipeline but can be changed during pipeline usage to control some aspect of rendering.
		 * When going for full performance this option should not be used but is provided nevertheless.
		 *
		 * @section Dynamic
		 * @param[in] states Parameter pack of VkDynamicState to use
		 */
		template <DynamicStateTrait... DynamicState>
		GraphicsPipelineBuilder& withDynamics(DynamicState... states) {
			dynamics = { states... };
			return *this;
		}

		/**
		 * Specify the dimensions of the viewport (Vulkan analogue of the OpenGL glViewport()),
		 * designates an rectangle that should at all times exactly match the screen to avoid image stretching.
		 *
		 * @section View
		 * @param[in] min_depth Specifies the lower bound for values that the depth field will assume
		 * @param[in] max_depth Specifies the upper bound for values that the depth field will assume
		 */
		GraphicsPipelineBuilder& withViewport(int x, int y, uint32_t width, uint32_t height, float min_depth = 0.0f, float max_depth = 1.0f);

		/**
		 * Specifies the dimensions of the scissor rectangle, that is a rectangle of screen pixels where the rendering
		 * will be applied. This is similar to the stencil buffer but instead of being able to specify an arbitrary
		 * pixel mask scissors designate a box.
		 *
		 * @section View
		 */
		GraphicsPipelineBuilder& withScissors(int x, int y, uint32_t width, uint32_t height);

		/**
		 * Adds a binding layout that specifies how vertex data is read and interpreted. Multiple
		 * bindings can specified for a single pipeline. Layouts should be created once and cached,
		 * the same object can be passed to multiple pipeline builders.
		 *
		 * @section Vertex
		 * @param[in] layout Vertex layout created using BindingLayoutBuilder
		 */
		GraphicsPipelineBuilder& withBindingLayout(BindingLayout& layout);

		/**
		 * Describes the layout (ordering and types) of descriptors sets (binding slots),
		 * a descriptor set constructed based on a layout can be used to pass resources to the shader
		 * by writing to its slots. Layouts should be created once and cached, the same object can
		 * be passed to multiple pipeline builders.
		 *
		 * @section Descriptor
		 * @param[in] layout Descriptor layout created using DescriptorSetLayoutBuilder
		 */
		GraphicsPipelineBuilder& withDescriptorSetLayout(const DescriptorSetLayout& layout);

		/**
		 * Describes how the vertices will be interpreted be assembled into rendering primitives,
		 * primitive restart should be kept disabled unless actually used. Primitive restart only affects
		 * drawing of indexed geometry.
		 *
		 * @section Assembly
		 * @param[in] topology       The type of the basic rendering primitive used (triangle, triangle_strip, line, etc.)
		 * @param[in] enable_restart Whether 'primitive restart' should be enabled for indexed geometry
		 */
		GraphicsPipelineBuilder& withPrimitive(VkPrimitiveTopology topology, bool enable_restart = false);

		/**
		 * If enabled clamps the fragments depth value to the range specified in the withViewport() call,
		 * so that the value is never lower than the min_depth or higher than the max_depth. By default this is
		 * disabled.
		 *
		 * @section Rasterizer
		 * @param[in] enable Enable fragment depth clamp
		 */
		GraphicsPipelineBuilder& withDepthClamp(bool enable);

		/**
		 * Controls a equation used to modify the depth output of fragment shaders, when enabled the
		 * fragment final depth value is modified according to this formula:
		 *
		 * @code
		 * max_slope = ceil(|dz / dx|, |dz / dy|)
		 * epsilon = (* minimum representable value > 0 in the depth-buffer format *)
		 * depth_bias = constant * epsilon + slope * max_slope;
		 * final_depth = fragment_depth + floor(|depth_bias|, clamp) * signum(depth_bias)
		 * @endcode
		 *
		 * The fancy calculus notation for the max slope in context of triangles resolved to the simple
		 * maximum slope of the triangle face along any of the two axis. By default depth bias is left disabled.
		 *
		 * @section Rasterizer
		 * @param[in] constant constant offset added to the depth, in epsilon units
		 * @param[in] clamp    maximal bias (both negative and positive)
		 * @param[in] slope    slope-multiplied depth bias
		 */
		GraphicsPipelineBuilder& withDepthBias(bool enable, float constant = 0.0f, float clamp = 0.0f, float slope = 0.0f);

		/**
		 * Controls how the selected primitive will actually be rasterized to the screen, primitive can either be
		 * drawn as a filled plane, as vertex-connecting lines, or vertex points. By default this is set to fill.
		 *
		 * @section Rasterizer
		 * @param[in] mode Mode to use
		 */
		GraphicsPipelineBuilder& withPolygonMode(VkPolygonMode mode);

		/**
		 * Relevant only for line polygons mode, specified the width of the lines used.
		 *
		 * @section Rasterizer
		 * @param[in] width Line width in pixels
		 */
		GraphicsPipelineBuilder& withLineWidth(float width);

		/**
		 * Enables or disables polygon culling, that is, the hiding of the 'back' face, in order to speed up rendering.
		 * If all the rendered geometry is already front-facing (either because it is already sorted, or consists of 2D
		 * front-facing GUI elements) enabling culling can actually degrade performance. Vulkan detect the front of a
		 * triangle based on the order of its vertices. For the best culling performance use clockwise triangles in
		 * 'back' mode. By default culling is disabled.
		 *
		 * @section Rasterizer
		 * @param[in] enable Enable culling
		 * @param[in] face   (Optional) Select the winding of the front face
		 * @param[in] mode   (Optional) Select which faces should be culled
		 */
		GraphicsPipelineBuilder& withCulling(bool enable, VkFrontFace face = VK_FRONT_FACE_CLOCKWISE, VkCullModeFlags mode = VK_CULL_MODE_BACK_BIT);

		/**
		 * Specifies the range in which depth value needs to fall for the depth test
		 * to be passed. Calling this function enabled depth bounds. Depth bounds are disabled by default.
		 *
		 * @section Depth
		 * @param[in] lower The lower depth value
		 * @param[in] upper The upper depth value
		 */
		GraphicsPipelineBuilder& withDepthBound(float lower, float upper);

		/**
		 * Enable the classical depth test and specify the function to use. Depth value is tested according
		 * to this formula:
		 *
		 * @code
		 * if (!read || (fragment_depth [function] stored_depth_value)) {
		 *     if (write) {
		 *         stored_depth_value = fragment_depth;
		 *     }
		 *
		 *     draw_fragment();
		 * }
		 * @endcode
		 *
		 * @section Depth
		 * @param[in] function Depth value comparator
		 * @param[in] read     Should the depth test be skipped?
		 * @param[in] write    Should the new value be saved?
		 */
		GraphicsPipelineBuilder& withDepthTest(VkCompareOp function, bool read, bool write);

		/**
		 * Specifies blend constants, those can be used by standard blending functions.
		 *
		 * @section Blending
		 * @param[in] r Red constant
		 * @param[in] g Green constant
		 * @param[in] b Blue constant
		 * @param[in] a Alpha constant
		 */
		GraphicsPipelineBuilder& withBlendConstants(float r, float g, float b, float a);

		/**
		 * Configures a binary blending function, specified by the formula:
		 *
		 * @code
		 * final_color = fragment_color & mask;
		 * @endcode
		 *
		 * @section Blending
		 * @param[in] mask Specifies which color channels should be kept
		 */
		GraphicsPipelineBuilder& withBlendWriteMask(VkColorComponentFlags mask);

		/**
		 * Configures a standard color blending function, specified by the formula:
		 *
		 * @code
		 * final_color.rgb = (src * fragment_color.rgb) [op] (dst * previous_color.rgb)
		 * @endcode
		 *
		 * @note For this to apply blending must be set to enabled!
		 *
		 * @section Blending
		 * @param[in] src Specifies how the color returned by the fragment should be treated
		 * @param[in] op  Specifies how the colors should be combined
		 * @param[in] dst Specifies how the old color should be treated
		 */
		GraphicsPipelineBuilder& withBlendColorFunc(VkBlendFactor src, VkBlendOp op, VkBlendFactor dst);

		/**
		 * Configures a standard alpha blending function, specified by the formula:
		 *
		 * @code
		 * final_color.a = (src * fragment_color.a) [op] (dst * previous_color.a)
		 * @endcode
		 *
		 * @note For this to apply blending must be set to enabled!
		 *
		 * @section Blending
		 * @param[in] src Specifies how the alpha returned by the fragment should be treated
		 * @param[in] op  Specifies how the alphas should be combined
		 * @param[in] dst Specifies how the old alpha should be treated
		 */
		GraphicsPipelineBuilder& withBlendAlphaFunc(VkBlendFactor src, VkBlendOp op, VkBlendFactor dst);

		/**
		 * Configures a binary blending function, specified by the formula:
		 *
		 * @code
		 * final_color = fragment_color [op] previous_color;
		 * @endcode
		 *
		 * @note For this to apply blending must be set to bitwise!
		 *
		 * @section Blending
		 * @param[in] op The binary function to use
		 */
		GraphicsPipelineBuilder& withBlendBitwiseFunc(VkLogicOp op);

		/**
		 * Specifies the blending mode (disabled, enabled, bitwise), enabling this is a prerequisite for using
		 * any blending function.
		 *
		 * @section Blending
		 * @param[in] mode The blending mode to use.
		 */
		GraphicsPipelineBuilder& withBlendMode(BlendMode mode);

		/**
		 * Connects this pipeline with render pass subpass it implements.
		 *
		 * @section Renderpass
		 * @param[in] render_pass   The render pass
		 * @param[in] subpass_index The index of the subpass withing the render pass
		 */
		GraphicsPipelineBuilder& withRenderPass(RenderPass& render_pass, int subpass_index);

		/**
		 * Add shaders that are to be used by this pipeline.
		 *
		 * @section Shader
		 * @param[in] shaders Compiled shaders to be used
		 */
		template<ShaderTrait... Shaders>
		GraphicsPipelineBuilder& withShaders(Shaders... shaders) {
			stages = { shaders... };
			return *this;
		}

	public:

		/// Complete building and create the pipeline
		GraphicsPipeline build();

};
