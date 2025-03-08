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

struct Vertex3D {
	vec3 position;
	uint8_t r, g, b, a;
	vec2 uv;
	uint materialIndex;
	uint padding;
};

struct RenderMaterial {
	uint index;
	vec4 albedo;
	vec3 emissive;
	vec3 specular;
	float shininess;
	uint albedoTextureIndex;
};

struct RenderObjectData {
	uint64_t vertexAddress;
	uint64_t indexAddress;
	mat4 portal;
};

layout(location = 0) rayPayloadInEXT HitPayload rPayload;
layout(location = 1) rayPayloadEXT bool rShadowPayload;

layout(buffer_reference, scalar) buffer Vertices { Vertex3D v[]; };
layout(buffer_reference, scalar) buffer Indices { ivec3 i[]; };
layout(binding = 0, set = 0) uniform accelerationStructureEXT TLAS;
layout(binding = 2, set = 0, scalar) uniform _SceneUniform { SceneUniform uSceneObject; };
layout(binding = 3, set = 0, scalar) buffer RenderObjectBuffer { RenderObjectData i[]; } renderObjectBuffer;
layout(binding = 4, set = 0) uniform sampler2D textures[];
layout(binding = 5, set = 0, scalar) buffer MaterialBuffer { RenderMaterial i[]; } materials;

hitAttributeEXT vec2 attribs;

// Returns true if the shadow ray hits an object (it means the point is in shadow)
bool traceShadowRay(vec3 origin, vec3 normal, vec3 shadowRayDirection) {

	vec3 shadowRayOrigin = origin + normal * 0.001;

	uint shadowRayFlags = gl_RayFlagsOpaqueEXT | gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsSkipClosestHitShaderEXT;

	// Initialize the shadow ray payload to false
	// If the ray misses, the payload will be set to true
	rShadowPayload = false;

	traceRayEXT(TLAS,          // acceleration structure
		shadowRayFlags,        // rayFlags
		0xFF,                  // cullMask
		0,                     // sbtRecordOffset
		0,                     // sbtRecordStride
		1,                     // missIndex
		shadowRayOrigin,       // ray origin
		0.001,                 // ray min range
		shadowRayDirection,    // ray direction
		10000.0,               // ray max range
		1                      // payload location
	);

	return !rShadowPayload;
}

vec3 computeDiffuse(vec3 lightDir, vec3 normal) {
	vec3 c = vec3(max(dot(normal, lightDir), 0.0));
	return c;
}

vec3 computeSpecular(RenderMaterial material, vec3 viewDirection, vec3 lightDirection, vec3 normal) {

	const float kPi        = 3.14159265;
	const float kShininess = max(material.shininess, 4.0);

	const float kEnergyConservation = (2.0 + kShininess) / (2.0 * kPi);
	vec3 V = normalize(-viewDirection);
	vec3 R = reflect(-lightDirection, normal);
	float specular = kEnergyConservation * pow(max(dot(V, R), 0.0), kShininess);

	return vec3(material.specular * specular);
}

void main() {

	/**
	 * Get the vertex data
	 */

	RenderObjectData renderObject = renderObjectBuffer.i[gl_InstanceCustomIndexEXT];
	Indices indices = Indices(renderObject.indexAddress);
	Vertices vertices = Vertices(renderObject.vertexAddress);

	const ivec3 index = indices.i[gl_PrimitiveID];

	Vertex3D v0 = vertices.v[index.x];
	Vertex3D v1 = vertices.v[index.y];
	Vertex3D v2 = vertices.v[index.z];

	const uint materialIndex = v0.materialIndex;
	RenderMaterial material = materials.i[materialIndex];

	/**
	 * Calculate hit point properties
	 */

	const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

	const vec3 position = v0.position * barycentrics.x + v1.position * barycentrics.y + v2.position * barycentrics.z;
	const vec3 positionWS = vec3(gl_ObjectToWorldEXT * vec4(position, 1.0));

	const vec2 uv = v0.uv * barycentrics.x + v1.uv * barycentrics.y + v2.uv * barycentrics.z;
	
	vec3 normal = vec3(v0.r, v0.g, v0.b) * barycentrics.x + vec3(v1.r, v1.g, v1.b) * barycentrics.y + vec3(v2.r, v2.g, v2.b) * barycentrics.z;
	normal = normalize((normal / 255.0) - 0.5);
	const vec3 normalWS = normalize(vec3(normal * gl_WorldToObjectEXT));

	/**
	 * Color
	 */

	vec3 color = vec3(0.0);
	vec3 lightCombined = vec3(0.0);

	// Check if the object isn't a portal
	if (renderObject.portal[0][0] == 0.0){

		vec3 diffuse = vec3(0.0);
		vec3 specular = vec3(0.0);

		vec3 lightDirection = normalize(uSceneObject.dirLightDirection);
		vec3 lightColor = uSceneObject.dirLightColor;

		bool inShadow = false;

		if (uSceneObject.shadows) {
			inShadow = traceShadowRay(positionWS, normalWS, lightDirection);
		}

		if (!inShadow) {
			diffuse = computeDiffuse(lightDirection, normalWS) * lightColor;
			specular = computeSpecular(material, gl_WorldRayDirectionEXT, lightDirection, normalWS) * lightColor;
		}
	
		color = clamp(mix(texture(textures[nonuniformEXT(material.albedoTextureIndex)], uv).rgb, material.emissive, length(material.emissive)), 0.0, 1.0);

		lightCombined = specular + diffuse + material.emissive + uSceneObject.ambientColor;
	}

	/**
	 * Set return values
	 */

	rPayload.hit = vec4(positionWS, 1.0);
	rPayload.color = vec4(color, 0.0);
	rPayload.light = vec4(lightCombined, 0.0);
	rPayload.normal = vec4(normalWS, 0.0);
	rPayload.specular = vec4(material.specular, material.shininess);
	rPayload.portal = renderObject.portal;
}
