#version 450

layout(location = 0) in vec4 vColor;
layout(location = 1) in vec2 vTexture;
layout(location = 0) out vec4 fColor;

layout(binding = 0) uniform sampler2D uAtlas;

void main() {
    vec4 texel = texture(uAtlas, vTexture / textureSize(uAtlas, 0));
    fColor = texel * vColor;
}