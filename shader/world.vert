#version 450
#extension GL_GOOGLE_include_directive : enable

#include "scene.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec4 iColor;
layout(location = 2) in vec2 iTexture;
layout(location = 3) in uint iMaterial; // unused

layout(location = 0) out vec4 vColor;
layout(location = 1) out vec2 vTexture;

layout(binding = 1, set = 0) uniform _SceneUniform { SceneUniform uSceneObject; };

layout(push_constant) uniform MeshConstant {
	mat4 matrix;
} uMeshObject;

void main() {
	mat4 matrix = uSceneObject.projection * uSceneObject.view * uMeshObject.matrix;

	gl_Position = matrix * vec4(iPosition, 1.0);
	vColor = iColor;
	vTexture = iTexture;
}