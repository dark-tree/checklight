#pragma once

// I had some issues with IDE being confused here
// so I added an explicit include guard here, don't replicate it!
#ifndef ABSTRACT_PIPELINE_HEADER
#define ABSTRACT_PIPELINE_HEADER

#include "external.hpp"
#include "render.hpp"
#include "render/vulkan/shader/shader.hpp"
#include "render/vulkan/descriptor/vertex.hpp"
#include "render/vulkan/descriptor/layout.hpp"
#include "render/vulkan/descriptor/push.hpp"

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

		/// get handle to the underlying pipeline object
		VkPipeline getHandle() const;

		/// Get handle to the underlying pipeline layout object
		VkPipelineLayout getLayout() const;

};

template <class Self>
class AbstractPipelineBuilder {

	protected:

		std::vector<VkDescriptorSetLayout> sets;
		std::vector<VkDynamicState> dynamics;
		std::vector<Shader> stages;

		VkPipelineDynamicStateCreateInfo dynamic {};
		VkPipelineLayoutCreateInfo layout {};
		VkPushConstantRange vk_constant {};
		VkDevice vk_device;

		void finalize() {
			dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamic.dynamicStateCount = (uint32_t) dynamics.size();
			dynamic.pDynamicStates = dynamics.data();

			layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			layout.setLayoutCount = sets.size();
			layout.pSetLayouts = sets.data();
			layout.pushConstantRangeCount = (vk_constant.size != 0);
			layout.pPushConstantRanges = &vk_constant;
		}

		VkPipelineLayout createPipelineLayout() {
			VkPipelineLayout pipeline_layout;

			if (vkCreatePipelineLayout(vk_device, &layout, nullptr, &pipeline_layout) != VK_SUCCESS) {
				throw std::runtime_error {"Failed to create graphics pipeline layout!"};
			}

			return pipeline_layout;
		}

	public:

		AbstractPipelineBuilder(const LogicalDevice& device)
		: vk_device(device.getHandle()) {}

		/**
		 * Adds a vulkan dynamic state, that is, a pipeline property that is not static to the
		 * pipeline but can be changed during pipeline usage to control some aspect of rendering.
		 * When going for full performance this option should not be used but is provided nevertheless.
		 *
		 * @section Dynamic
		 * @param[in] states Parameter pack of VkDynamicState to use
		 */
		template <DynamicStateTrait... DynamicState>
		Self& withDynamics(DynamicState... states) {
			dynamics = { states... };
			return static_cast<Self&>(*this);
		}

		/**
		 * Describes the layout (ordering and types) of descriptors sets (binding slots),
		 * a descriptor set constructed based on a layout can be used to pass resources to the shader
		 * by writing to its slots. Layouts should be created once and cached, the same object can
		 * be passed to multiple pipeline builders.
		 *
		 * @section Descriptor
		 * @param[in] layout Descriptor layout created using DescriptorSetLayoutBuilder
		 */
		Self& withDescriptorSetLayout(const DescriptorSetLayout& layout) {
			sets.push_back(layout.getHandle());
			return static_cast<Self&>(*this);
		}

		/**
		 * Adds the specified push constant to the pipeline, you can only add one push
		 * constant to a pipeline. Push constants are the fastest way to pass per-draw call
		 * data to the driver (other than instance buffers, but those can't be changed once
		 * render pass begins). The only limiting factor of push constants is that they come in very
		 * small sizes (only 128 bytes guaranteed by Vulkan 1.0 standard) and can't contains buffer
		 * or texture references.
		 *
		 * @section Descriptor
		 * @param[in] constant The push constant to add to the pipeline
		 */
		Self& withPushConstant(const PushConstant& constant) {
			this->vk_constant = constant.getRange();
			return static_cast<Self&>(*this);
		}

		/**
		 * Add shaders that are to be used by this pipeline.
		 *
		 * @section Shader
		 * @param[in] shaders Compiled shaders to be used
		 */
		template<ShaderTrait... Shaders>
		Self& withShaders(Shaders... shaders) {
			stages = { shaders... };
			return static_cast<Self&>(*this);
		}

};

// to avoid compatibility nightmare
#include "graphics.hpp"
#include "raytrace.hpp"

// ABSTRACT_PIPELINE_HEADER
#endif
