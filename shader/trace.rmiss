#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int8 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_GOOGLE_include_directive : enable

#include "raycommon.glsl"
#include "scene.glsl"

layout(location = 0) rayPayloadInEXT HitPayload rPayload;

layout(binding = 2, set = 0, scalar) uniform _SceneUniform { SceneUniform uSceneObject; };

void main(){
	rPayload.hit = vec4(0.0, 0.0, 0.0, 0.0);
	rPayload.color = vec4(uSceneObject.skyColor, 0.0);
	rPayload.light = vec4(uSceneObject.skyEmission);
	rPayload.normal = vec4(0.0, 0.0, 1.0, 0.0);
	rPayload.portal = mat4(0.0);
}