#version 450

layout(binding = 0) uniform sampler2D uAlbedoSampler;
layout(binding = 1) uniform sampler2D uIlluminationSampler;
layout(binding = 2) uniform sampler2D uNormalSampler;

layout(location = 0) in vec2 vTexture;
layout(location = 0) out vec4 fColor;
layout(location = 1) out vec4 fPrevIllumination;
layout(location = 2) out vec4 fPrevDepth;

#define FILTER_RADIUS 1
#define STEP_SIZE 1

float normalEdgeStoppingWeight(vec3 centerNormal, vec3 sampleNormal, float power) {
    return pow(clamp(dot(centerNormal, sampleNormal), 0.0, 1.0), power);
}

float depthEdgeStoppingWeight(float centerDepth, float sampleDepth, float phi) {
    return exp(-abs(centerDepth - sampleDepth) / phi);
}

void main() {
    
    vec4 centerIllumSample = texture(uIlluminationSampler, vTexture);
    vec4 centerNormal = texture(uNormalSampler, vTexture);
    float centerDepth = centerNormal.w;

    vec3 illumSum = centerIllumSample.rgb;
    float weightSum = 1.0;

    float illumTexelSize = 1.0 / float(textureSize(uIlluminationSampler, 0).x);

    for (int x = -FILTER_RADIUS; x <= FILTER_RADIUS; x++) {
        for (int y = -FILTER_RADIUS; y <= FILTER_RADIUS; y++) {
            if (x == 0 && y == 0) {
                continue;
            }

            vec2 pos = vTexture + vec2(x, y) * illumTexelSize * STEP_SIZE;

            vec4 currNormal = texture(uNormalSampler, pos);
            float currDepth = currNormal.w;

            float weightNormal = normalEdgeStoppingWeight(centerNormal.xyz, currNormal.xyz, 128.0);
            float weightDepth = depthEdgeStoppingWeight(centerDepth, currDepth, 0.00001);
            
            illumSum += texture(uIlluminationSampler, pos).rgb * weightNormal * weightDepth;
            weightSum += weightNormal * weightDepth;
        }
    }

    illumSum /= weightSum;

    vec4 color = texture(uAlbedoSampler, vTexture);

    fColor = vec4(color.rgb * illumSum, 1.0);
    fPrevDepth = vec4(centerNormal.xyz, color.w);

    fPrevIllumination = texture(uIlluminationSampler, vTexture);

    gl_FragDepth = color.w;  
}