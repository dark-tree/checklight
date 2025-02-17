#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int8 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

struct HitPayload {
    vec3 value;
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
    
    const vec3 normal = vec3(v0.r, v0.g, v0.b) * barycentrics.x + vec3(v1.r, v1.g, v1.b) * barycentrics.y + vec3(v2.r, v2.g, v2.b) * barycentrics.z;

    rPayload.value = normal/255;
}
