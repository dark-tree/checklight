#version 450
#extension GL_GOOGLE_include_directive : enable

layout(binding = 0) uniform sampler2D uAlbedoSampler;
layout(binding = 1) uniform sampler2D uIlluminationSampler;
layout(binding = 2) uniform sampler2D uNormalSampler;
layout(binding = 3) uniform sampler2D uSolidIlluminationSampler;

layout(location = 0) in vec2 vTexture;
layout(location = 0) out vec4 fColor;
layout(location = 1) out vec4 fPrevDepth;

#include "denoisecommon.glsl"

#define FILTER_RADIUS 1
#define STEP_SIZE 9

void main() {
	
	vec4 centerIllumSample = texture(uIlluminationSampler, vTexture);
	vec4 centerNormal = texture(uNormalSampler, vTexture);
	float centerDepth = centerNormal.w;

	float ageWeight = (200.0 - clamp(centerIllumSample.w - 100, 0, 200)) / 200.0;

	vec4 illumSum = vec4(centerIllumSample.rgb, 1.0);
	illumSum += edgeAvoidingBlur(FILTER_RADIUS, STEP_SIZE, vTexture, centerDepth, centerNormal.xyz, ageWeight, uNormalSampler, uIlluminationSampler);  
	illumSum /= illumSum.w;

	vec4 color = texture(uAlbedoSampler, vTexture);
	vec3 solidIllumination = texture(uSolidIlluminationSampler, vTexture).rgb;

	fColor = vec4(color.rgb * (illumSum.rgb + solidIllumination), 1.0);
	fPrevDepth = vec4(centerNormal.xyz, color.w);

	gl_FragDepth = color.w;  
}
