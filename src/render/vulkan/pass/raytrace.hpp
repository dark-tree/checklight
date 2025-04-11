#pragma once

#include "pipeline.hpp"
#include "render/vulkan/shader/table.hpp"

class RaytracePipelineBuilder : public AbstractPipelineBuilder<RaytracePipelineBuilder> {

	private:

		int limit = 1;
		int depth = 1;
		ShaderTableLayout layout;

	public:

		RaytracePipelineBuilder(const LogicalDevice& device);
		static RaytracePipelineBuilder of(const LogicalDevice& device);

		/**
		 * Specifies the number of recursive ray trace invocations,
		 * value of 1 is guaranteed to be supported and specifies that the
		 * initial ray can be invoked, but nothing beside that. Only values >= 2
		 * signify actual recursion being used.
		 *
		 * @note
		 * The spec does not guarantee a recursion check at runtime.
		 * If you exceed either the recursion depth you reported in the raytrace pipeline create info,
		 * or the physical device recursion limit, undefined behavior results.
		 *
		 * @section Raytrace
		 * @param[in] depth Maximum number of recursive ray invocations
		 */
		RaytracePipelineBuilder& withRecursionDepth(int depth);

		/**
		 * Specifies the shader table to be used by the raytracing pipeline,
		 * shader table consists of 3 regions - generate, miss, and hit (in
		 * this order). The hit group number to use is specified using
		 * hit group offset in RenderObject's instance.
		 *
		 * @section Raytrace
		 * @param layout The layout, as created with ShaderArrayBuilder
		 */
		RaytracePipelineBuilder& withShaderLayout(const ShaderTableLayout& layout);

	public:

		/// Complete building and create the pipeline
		GraphicsPipeline build();

};