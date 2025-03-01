#version 460
#extension GL_EXT_ray_tracing : require

struct HitPayload {
	// posX, posY, posZ, hit
	vec4 hit;
	// colorR, colorG, colorB
	vec4 color;
	// lightR, lightG, lightB
	vec4 light;
	// normalX, normalY, normalZ
	vec4 normal;
	// specularR, specularG, specularB, shininess
	vec4 specular;
};

layout(location = 0) rayPayloadInEXT HitPayload rPayload;

void main(){
	rPayload.hit = vec4(0.0, 0.0, 0.0, 0.0);
    rPayload.color = vec4(0.0, 0.1, 0.3, 0.0);
	rPayload.light = vec4(0.2);
    rPayload.normal = vec4(0.0, 0.0, 1.0, 0.0);
}