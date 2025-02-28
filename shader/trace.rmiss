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

layout(location = 0) rayPayloadInEXT HitPayload rPayload;

void main(){
	rPayload.hit = vec4(0.0, 0.0, 0.0, 0.0);
    rPayload.value = vec4(0.0, 0.1, 0.3, 0.0);
    rPayload.normal = vec4(0.0, 0.0, 1.0, 0.0);
}