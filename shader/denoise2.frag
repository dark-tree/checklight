#version 450
#extension GL_GOOGLE_include_directive : enable

layout(binding = 0) uniform sampler2D uIlluminationSampler;
layout(binding = 1) uniform sampler2D uNormalSampler;
layout(binding = 2) uniform sampler2D uPositionSampler;

layout(location = 0) in vec2 vTexture;
layout(location = 0) out vec4 fIllumination;

#include "denoisecommon.glsl"

#define FILTER_RADIUS 1
#define STEP_SIZE 3

void main() {
	
	vec4 centerIllumSample = texture(uIlluminationSampler, vTexture);
	vec4 centerNormal = texture(uNormalSampler, vTexture);
	float centerDepth = centerNormal.w;
	vec3 centerPositionWS = texture(uPositionSampler, vTexture).xyz;

	vec4 illumSum = vec4(centerIllumSample.rgb, 1.0);

	illumSum += edgeAvoidingBlur(
		FILTER_RADIUS, 
		STEP_SIZE, 
		vTexture,
		centerDepth,
		centerNormal.xyz, 
		1.0, 
		uNormalSampler,
		uIlluminationSampler,
		uPositionSampler, 
		0.0,
		centerPositionWS
	);

	illumSum /= illumSum.w;

	fIllumination = vec4(illumSum.rgb, centerIllumSample.w);
}
