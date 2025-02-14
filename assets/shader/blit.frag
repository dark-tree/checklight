#version 450

layout(binding = 0) uniform sampler2D uAlbedoSampler;

layout(location = 0) in vec2 vTexture;
layout(location = 0) out vec4 fColor;

void main() {
    fColor = texture(uAlbedoSampler, vTexture);
}