#version 450
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

#include "scene.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec4 iColor;
layout(location = 2) in vec2 iTexture;
layout(location = 3) in uint iMaterial;

layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec2 vTexture;
layout(location = 2) out flat uint vMaterialIndex;
layout(location = 3) out vec3 vWorldPos;

layout(binding = 1, set = 0, scalar) uniform _SceneUniform { SceneUniform uSceneObject; };

struct InstanceData {
	mat4 model;
};

layout(binding = 2) readonly buffer InstanceBuffer {
	InstanceData instances[];
};

layout(push_constant) uniform Push {
	uint instanceOffset;
} push;

void main() {
	uint offset = push.instanceOffset + gl_InstanceIndex;
	InstanceData inst = instances[offset];

	mat4 matrix = uSceneObject.projection * uSceneObject.view * inst.model;

	vWorldPos = vec3(inst.model * vec4(iPosition, 1.0));
	gl_Position = matrix * vec4(iPosition, 1.0);
	vec3 localNormal = vec3(iColor-.5);
	vNormal = normalize(mat3(inst.model) * localNormal);
	vTexture = iTexture;
	vMaterialIndex = iMaterial;
}