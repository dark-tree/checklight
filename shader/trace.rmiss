#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable

#include "raycommon.glsl"

layout(location = 0) rayPayloadInEXT HitPayload rPayload;

void main(){
	rPayload.hit = vec4(0.0, 0.0, 0.0, 0.0);
	rPayload.color = vec4(0.0, 0.1, 0.3, 0.0);
	rPayload.light = vec4(0.2);
	rPayload.normal = vec4(0.0, 0.0, 1.0, 0.0);
	rPayload.portal = mat4(0.0);
}