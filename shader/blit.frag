#version 450

layout(binding = 0) uniform sampler2D uAlbedoSampler;
layout(binding = 1) uniform sampler2D uIlluminationSampler;

layout(location = 0) in vec2 vTexture;
layout(location = 0) out vec4 fColor;
layout(location = 1) out vec4 fPrevIllumination;

void main() {
    vec4 texel = texture(uAlbedoSampler, vTexture);

    fPrevIllumination = texture(uIlluminationSampler, vTexture);
    fColor = vec4(texel.rgb, 1.0f);
    gl_FragDepth = texel.w;  
}