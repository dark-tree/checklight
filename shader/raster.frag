#version 450

layout(location = 0) in vec4 vColor;
layout(location = 1) in vec2 vTexture;
layout(location = 0) out vec4 fColor;

layout(binding = 1) uniform sampler2D uAtlas;

void main() {
	ivec2 textureSize = textureSize(uAtlas, 0);
    
    vec2 normalizedTexCoords = vTexture / vec2(textureSize);
    
    fColor = vec4(normalizedTexCoords,1.0,0.0) * vColor;
}