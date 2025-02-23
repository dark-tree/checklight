#version 460
#extension GL_EXT_ray_tracing : require

struct HitPayload {
    // posX, posY, posZ, hit
    vec4 hit;
    // colorR, colorG, colorB
    vec4 value;
    // normalX, normalY, normalZ
    vec4 normal;
};

layout(location = 0) rayPayloadEXT HitPayload rPrimaryPayload;
layout(location = 1) rayPayloadEXT bool rShadowPayload;

layout(binding = 0, set = 0) uniform accelerationStructureEXT TLAS;
layout(binding = 1, set = 0, rgba32f) uniform image2D image;
layout(binding = 6, set = 0, r32f) uniform image2D depth;

layout(binding = 2, set = 0) uniform SceneUniform {
	mat4 view;
	mat4 projection;
} uSceneObject;

void main() {
	const vec2 pixelCenter = vec2(gl_LaunchIDEXT.xy) + vec2(0.5);
	const vec2 inUV = pixelCenter/vec2(gl_LaunchSizeEXT.xy);
	vec2 d = inUV * 2.0 - 1.0;

	mat4 viewInverse = inverse(uSceneObject.view);
	mat4 projInverse = inverse(uSceneObject.projection);
	
	/**
	* Primary ray
	*/

	vec4 origin = viewInverse * vec4(0, 0, 0, 1);
	vec4 target = projInverse * vec4(d.x, d.y, 1, 1);
	vec4 direction = viewInverse * vec4(normalize(target.xyz), 0);

	float tMin = 0.001;
	float tMax = 10000.0;
	uint primaryRayFlags = gl_RayFlagsOpaqueEXT;

	traceRayEXT(TLAS,			// acceleration structure
		primaryRayFlags,		// rayFlags
		0xFF,					// cullMask
		0,						// sbtRecordOffset
		0,						// sbtRecordStride
		0,						// missIndex
		origin.xyz,				// ray origin
		tMin,					// ray min range
		direction.xyz,			// ray direction
		tMax,					// ray max range
		0						// payload location
	);

	vec3 primaryColor = rPrimaryPayload.value.xyz;
	float depthValue = tMax;

	/**
	* Directional light shadow ray
	*/

	if (rPrimaryPayload.hit.w > 0) {

		// Calculate depth value
		depthValue = length(rPrimaryPayload.hit.xyz - origin.xyz);

		// Shadow ray
		vec3 shadowRayDirection = normalize(vec3(0.0, 0.5, -1));
		vec3 shadowRayOrigin = rPrimaryPayload.hit.xyz + rPrimaryPayload.normal.xyz * 0.001;

		uint shadowRayFlags = gl_RayFlagsOpaqueEXT | gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsSkipClosestHitShaderEXT;

		rShadowPayload = false;

		traceRayEXT(TLAS,			// acceleration structure
			shadowRayFlags,			// rayFlags
			0xFF,					// cullMask
			0,						// sbtRecordOffset
			0,						// sbtRecordStride
			1,						// missIndex
			shadowRayOrigin,		// ray origin
			0.001,					// ray min range
			shadowRayDirection,		// ray direction
			10000.0,				// ray max range
			1						// payload location
		);

		if (!rShadowPayload) {
			primaryColor *= 0.3;
		}
	}

	/**
	* Write to the output image
	*/

	imageStore(image, ivec2(gl_LaunchIDEXT.xy), vec4(primaryColor, 1.0));
	imageStore(depth, ivec2(gl_LaunchIDEXT.xy), vec4(depthValue) / tMax);
}
