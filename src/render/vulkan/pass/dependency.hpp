#pragma once

#include "external.hpp"

class RenderPassBuilder;

/**
 * Create a dependency between two locations in the pipeline,
 * that dependency is specified using two subpass indices, two pipeline stages, and two actions
 */
class DependencyBuilder {

	private:

		RenderPassBuilder& builder;
		VkSubpassDependency description {};

		friend class RenderPassBuilder;

		VkSubpassDependency finalize();

	public:

		DependencyBuilder(RenderPassBuilder& builder, VkDependencyFlags flags);

		/**
		 * @brief Specify the point that must be reached
		 *
		 * Specify, the operation of a stage, of a subpass, that will be waited upon,
		 * before the operation defined using then() can begin execution, this operation must be completed.
		 *
		 * @param subpass the index of the subpass in which is the stage and operation to wait for
		 * @param stage the stage in which is the operation to wait for
		 * @param access the operation to wait for, pass 0 to just wait for stage
		 */
		DependencyBuilder& first(uint32_t subpass, VkPipelineStageFlags stage, VkAccessFlags access);

		/**
		 * @brief Specify the point that must wait
		 *
		 * Specify, the operation of a stage, of a subpass, that will wait on this dependency,
		 * before this operation can begin execution, the point defined using first() must be completed.
		 *
		 * @param subpass the index of the subpass in which is the stage and operation to wait for
		 * @param stage the stage in which is the operation to wait for
		 * @param access the operation to wait for, pass 0 to just wait for stage
		 */
		DependencyBuilder& then(uint32_t subpass, VkPipelineStageFlags stage, VkAccessFlags access);

		/// Continue building the Render Pass
		RenderPassBuilder& next();

};