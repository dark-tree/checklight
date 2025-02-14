#pragma once

#include "external.hpp"

class Shader;
class ShaderTableLayout;
class ShaderHitGroup;

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

		ShaderTableBuilder& addRayGenShader(const Shader& shader);
		ShaderTableBuilder& addMissShader(const Shader& shader);
		ShaderHitGroup addHitGroup();

		ShaderTableLayout build() const;

};

class ShaderHitGroup {

	private:

		ShaderTableBuilder* builder;
		VkRayTracingShaderGroupCreateInfoKHR* group;

	public:

		ShaderHitGroup(ShaderTableBuilder* builder, VkRayTracingShaderGroupCreateInfoKHR* group);

		ShaderHitGroup& withClosestHit(const Shader& shader);
		ShaderHitGroup& withAnyHit(const Shader& shader);
		ShaderHitGroup& withIntersection(const Shader& shader);

};