
#include "group.hpp"
#include "shader.hpp"
#include "table.hpp"

/*
 * ShaderArrayBuilder
 */

int ShaderTableBuilder::getShaderReference(const Shader& shader) {
	auto handle = shader.getHandle();
	auto it = cache.find(handle);

	if (it != cache.end()) {
		return it->second;
	}

	int last = stages.size();
	stages.emplace_back(shader.getConfig());
	cache[handle] = last;
	return last;
}

VkRayTracingShaderGroupCreateInfoKHR ShaderTableBuilder::getEmptyGroup() const {
	VkRayTracingShaderGroupCreateInfoKHR group {};
	group.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	group.pNext = nullptr;
	group.generalShader = VK_SHADER_UNUSED_KHR;
	group.intersectionShader = VK_SHADER_UNUSED_KHR;
	group.anyHitShader = VK_SHADER_UNUSED_KHR;
	group.closestHitShader = VK_SHADER_UNUSED_KHR;

	return group;
}

ShaderTableBuilder& ShaderTableBuilder::addRayGenShader(const Shader& shader) {
	VkRayTracingShaderGroupCreateInfoKHR group = getEmptyGroup();
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = getShaderReference(shader);

	group_generate.push_back(group);
	return *this;
}

ShaderTableBuilder& ShaderTableBuilder::addMissShader(const Shader& shader) {
	VkRayTracingShaderGroupCreateInfoKHR group = getEmptyGroup();
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	group.generalShader = getShaderReference(shader);

	group_miss.push_back(group);
	return *this;
}

ShaderHitGroup ShaderTableBuilder::addHitGroup() {
	VkRayTracingShaderGroupCreateInfoKHR group = getEmptyGroup();
	group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;

	group_hit.push_back(group);
	return {this, &group_hit.back()};
}

ShaderTableLayout ShaderTableBuilder::build() const {
	ShaderTableLayout array {};

	array.generate = group_generate.size();
	array.miss = group_miss.size();
	array.hit = group_hit.size();

	array.groups.insert(array.groups.end(), group_generate.begin(), group_generate.end());
	array.groups.insert(array.groups.end(), group_miss.begin(), group_miss.end());
	array.groups.insert(array.groups.end(), group_hit.begin(), group_hit.end());

	array.stages = stages;

	return array;
}

/*
 * ShaderHitGroup
 */

ShaderHitGroup::ShaderHitGroup(ShaderTableBuilder* builder, VkRayTracingShaderGroupCreateInfoKHR* group)
: builder(builder), group(group) {}

ShaderHitGroup& ShaderHitGroup::withClosestHit(const Shader& shader) {
	group->closestHitShader = builder->getShaderReference(shader);
	return *this;
}

ShaderHitGroup& ShaderHitGroup::withAnyHit(const Shader& shader) {
	group->anyHitShader = builder->getShaderReference(shader);
	return *this;
}

ShaderHitGroup& ShaderHitGroup::withIntersection(const Shader& shader) {
	group->intersectionShader = builder->getShaderReference(shader);
	group->type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
	return *this;
}
