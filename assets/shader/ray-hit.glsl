#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable

struct HitPayload {
    vec3 value;
};

layout(location = 0) rayPayloadInEXT HitPayload rPayload;
hitAttributeEXT vec3 attribs;

void main() {
    rPayload.value = vec3(0.2, 0.5, 0.5);
}
