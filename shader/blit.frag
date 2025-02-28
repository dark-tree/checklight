#version 450

layout(binding = 0) uniform sampler2D uAlbedoSampler;

layout(location = 0) in vec2 vTexture;
layout(location = 0) out vec4 fColor;

void main() {
    vec4 texel = texture(uAlbedoSampler, vTexture);

    fColor = vec4(texel.rgb, 1.0f);
    gl_FragDepth = texel.w;
}