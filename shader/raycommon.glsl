
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

float hash1(inout float seed) {
	return fract(sin(seed += 0.1) * 43758.5453123);
}

vec2 hash2(inout float seed) {
	return fract(sin(vec2(seed += 0.1, seed += 0.1)) * vec2(43758.5453123, 22578.1459123));
}

vec3 hash3(inout float seed) {
	return fract(sin(vec3(seed += 0.1, seed += 0.1, seed += 0.1)) * vec3(43758.5453123, 22578.1459123, 19642.3490423));
}

vec3 cosWeightedRandomHemisphereDirection(const vec3 n, inout float seed) {
	vec2 r = hash2(seed);

	vec3  uu = normalize(cross(n, vec3(0.0, 1.0, 1.0)));
	vec3  vv = cross(uu, n);

	float ra = sqrt(r.y);
	float rx = ra * cos(6.2831 * r.x);
	float ry = ra * sin(6.2831 * r.x);
	float rz = sqrt(1.0 - r.y);
	vec3  rr = vec3(rx * uu + ry * vv + rz * n);

	return normalize(rr);
}

vec3 randomSphereDirection(inout float seed) {
	vec2 h = hash2(seed) * vec2(2., 6.28318530718) - vec2(1, 0);
	float phi = h.y;
	return vec3(sqrt(1. - h.x * h.x) * vec2(sin(phi), cos(phi)), h.x);
}
