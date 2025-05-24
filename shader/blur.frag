#version 450
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

#include "scene.glsl"

layout(binding = 0) uniform sampler2D uAlbedoSampler;
layout(binding = 1) uniform sampler2D uIlluminationSampler;
layout(binding = 2) uniform sampler2D uNormalSampler;
layout(binding = 3) uniform sampler2D uSolidIlluminationSampler;
layout(binding = 4, scalar) uniform _SceneUniform { SceneUniform uSceneObject; };
layout(binding = 5) uniform sampler2D uPositionSampler;

layout(location = 0) in vec2 vTexture;
layout(location = 0) out vec4 fColor;
layout(location = 1) out vec4 fPrevNormal;
layout(location = 2) out vec4 fPrevPosition;

#include "denoisecommon.glsl"

#define FILTER_RADIUS 1
#define STEP_SIZE 9

void main() {
	
	vec4 centerIllumSample = texture(uIlluminationSampler, vTexture);
	vec4 centerNormal = texture(uNormalSampler, vTexture);
	float centerDepth = centerNormal.w;
	vec3 centerPositionWS = texture(uPositionSampler, vTexture).xyz;

	vec4 color = texture(uAlbedoSampler, vTexture);
	vec4 illumSum = vec4(centerIllumSample.rgb, 1.0);

	if (uSceneObject.denoise){

		illumSum += edgeAvoidingBlur(
			FILTER_RADIUS, 
			STEP_SIZE, 
			vTexture,
			centerDepth,
			centerNormal.xyz, 
			uNormalSampler,
			uIlluminationSampler,
			uPositionSampler, 
			centerPositionWS
		);
		
		illumSum /= illumSum.w;
	}
	
	vec3 solidIllumination = texture(uSolidIlluminationSampler, vTexture).rgb;

	fColor = vec4(color.rgb * (illumSum.rgb + solidIllumination), 1.0);
	fPrevNormal = vec4(centerNormal.xyz, color.w);
	fPrevPosition = texture(uPositionSampler, vTexture);

	gl_FragDepth = color.w;  
}
