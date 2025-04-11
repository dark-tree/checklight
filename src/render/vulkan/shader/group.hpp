#pragma once

#include "external.hpp"

class Shader;
class ShaderTableLayout;
class ShaderHitGroup;

/**
 * Construct the sets of shaders to use during
 * raytracing operations, there can be multiple miss and generate shaders
 * as well as multiple hit groups (sets of shaders to use when a ray hits
 * some geometry) The offset into hit group table is dictated by the RenderObject
 * shader offset as well as the raygen shader itself.
 */
class ShaderTableBuilder {

	private:

		friend class ShaderHitGroup;

		std::vector<VkPipelineShaderStageCreateInfo> stages;
		std::unordered_map<VkShaderModule, int> cache;

		std::vector<VkRayTracingShaderGroupCreateInfoKHR> group_generate;
		std::vector<VkRayTracingShaderGroupCreateInfoKHR> group_miss;
		std::vector<VkRayTracingShaderGroupCreateInfoKHR> group_hit;

		int getShaderReference(const Shader& shader);
		VkRayTracingShaderGroupCreateInfoKHR getEmptyGroup() const;

	public:

		/// Add raygen shader, there should be at least one
		ShaderTableBuilder& addRayGenShader(const Shader& shader);

		/// Add miss shader, there should be at least one
		ShaderTableBuilder& addMissShader(const Shader& shader);

		/// The hit group, there should be at least one
		ShaderHitGroup addHitGroup();

		/// Create the final layout
		ShaderTableLayout build() const;

};

class ShaderHitGroup {

	private:

		ShaderTableBuilder* builder;
		VkRayTracingShaderGroupCreateInfoKHR* group;

	public:

		ShaderHitGroup(ShaderTableBuilder* builder, VkRayTracingShaderGroupCreateInfoKHR* group);

		/// Set the shader invoked after finding the closest to the camera triangle-ray intersection
		ShaderHitGroup& withClosestHit(const Shader& shader);

		/// Set the shader invoked for every triangle-ray intersection
		ShaderHitGroup& withAnyHit(const Shader& shader);

		/// Set the intersection shader for procedural geometry
		ShaderHitGroup& withIntersection(const Shader& shader);

};