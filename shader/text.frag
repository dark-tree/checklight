#version 450

layout(location = 0) in vec4 vColor;
layout(location = 1) in vec2 vTexture;
layout(location = 0) out vec4 fColor;

layout(binding = 0) uniform sampler2D uAtlas;

float median(float r, float g, float b) {
	return max(min(r, g), min(max(r, g), b));
}

// font_size / 64 * 6
float range() {
	vec2 unitRange = vec2(6) / vec2(textureSize(uAtlas, 0));
	vec2 screenTexSize = vec2(1.0) / fwidth(vTexture / textureSize(uAtlas, 0));
	return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
	vec4 fields = texture(uAtlas, vTexture / textureSize(uAtlas, 0));
	float distance = median(fields.x, fields.y, fields.z);
	float screen = range() * (distance - 0.5);
	float opacity = clamp(screen + 0.5, 0.0, 1.0);
	fColor = vec4(vColor.rgb, vColor.a * opacity);
}