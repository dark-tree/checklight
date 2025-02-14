#version 460
#extension GL_EXT_ray_tracing : require

struct HitPayload {
    vec3 value;
};

layout(location = 0) rayPayloadInEXT HitPayload rPayload;

void main(){
    rPayload.value = vec3(0.0, 0.1, 0.3);
}