#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int8 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

struct HitPayload {
    // posX, posY, posZ, hit
    vec4 hit;
    // colorR, colorG, colorB
    vec4 value;
    // normalX, normalY, normalZ
    vec4 normal;
};

struct Vertex3D {
	float x, y, z;
	uint8_t r, g, b, a;
    float u, v;
};

struct RenderObjectData {
    uint64_t vertexAddress;
    uint64_t indexAddress;
};

layout(location = 0) rayPayloadInEXT HitPayload rPayload;

layout(buffer_reference, scalar) buffer Vertices { Vertex3D v[]; };
layout(buffer_reference, scalar) buffer Indices { ivec3 i[]; };
layout(binding = 3, set = 0, scalar) buffer RenderObjectBuffer {
	RenderObjectData i[];
} renderObjectBuffer;
layout(binding = 4, set = 0) uniform sampler2D textures[];

hitAttributeEXT vec2 attribs;

void main() {
    RenderObjectData renderObject = renderObjectBuffer.i[gl_InstanceCustomIndexEXT];
    Indices indices = Indices(renderObject.indexAddress);
    Vertices vertices = Vertices(renderObject.vertexAddress);

    ivec3 index = indices.i[gl_PrimitiveID];

    Vertex3D v0 = vertices.v[index.x];
    Vertex3D v1 = vertices.v[index.y];
    Vertex3D v2 = vertices.v[index.z];

    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
    
    vec3 normal = vec3(v0.r, v0.g, v0.b) * barycentrics.x + vec3(v1.r, v1.g, v1.b) * barycentrics.y + vec3(v2.r, v2.g, v2.b) * barycentrics.z;
    normal = normalize((normal / 255.0) - 0.5);

    vec3 position = vec3(v0.x, v0.y, v0.z) * barycentrics.x + vec3(v1.x, v1.y, v1.z) * barycentrics.y + vec3(v2.x, v2.y, v2.z) * barycentrics.z;

	vec2 uv = vec2(v0.u, v0.v) * barycentrics.x + vec2(v1.u, v1.v) * barycentrics.y + vec2(v2.u, v2.v) * barycentrics.z;
	vec3 textureColor = texture(textures[1], uv).rgb;

    rPayload.hit = vec4(position, 1.0);
    rPayload.value = vec4(textureColor, 0.0);
    rPayload.normal = vec4(normal, 0.0);
}
