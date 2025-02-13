#pragma once

#include "external.hpp"

class Shader;
class ShaderHitGroup;

class ShaderArrayLayout {

	private:

		friend class ShaderArrayBuilder;
		friend class RaytracePipelineBuilder;

		int generate, miss, hit;

		std::vector<VkPipelineShaderStageCreateInfo> stages;
		std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups;

};

class ShaderArrayBuilder {

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

		ShaderArrayBuilder& addRayGenShader(const Shader& shader);
		ShaderArrayBuilder& addMissShader(const Shader& shader);
		ShaderHitGroup addHitGroup();

		ShaderArrayLayout build() const;

};

class ShaderHitGroup {

	private:

		ShaderArrayBuilder* builder;
		VkRayTracingShaderGroupCreateInfoKHR* group;

	public:

		ShaderHitGroup(ShaderArrayBuilder* builder, VkRayTracingShaderGroupCreateInfoKHR* group);

		ShaderHitGroup& withClosestHit(const Shader& shader);
		ShaderHitGroup& withAnyHit(const Shader& shader);
		ShaderHitGroup& withIntersection(const Shader& shader);

};