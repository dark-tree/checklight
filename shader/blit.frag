#version 450

layout(binding = 0) uniform sampler2D uAlbedoSampler;
layout(binding = 1) uniform sampler2D uIlluminationSampler;
layout(binding = 2) uniform sampler2D uNormalSampler;

layout(location = 0) in vec2 vTexture;
layout(location = 0) out vec4 fColor;
layout(location = 1) out vec4 fPrevIllumination;

#define FILTER_RADIUS 3
#define STEP_SIZE 1

float normalEdgeStoppingWeight(vec3 center_normal, vec3 sample_normal, float power) {
    return pow(clamp(dot(center_normal, sample_normal), 0.0f, 1.0f), power);
}

float depthEdgeStoppingWeight(float center_depth, float sample_depth, float phi) {
    return exp(-abs(center_depth - sample_depth) / phi);
}

void main() {
    
    vec4 centerIllumSample = texture(uIlluminationSampler, vTexture);
    vec4 centerNormal = texture(uNormalSampler, vTexture);
    float centerDepth = centerNormal.w;

    vec3 illumSum = centerIllumSample.rgb;
    float weightSum = 1.0f;

    float illumTexelSize = 1.0f / float(textureSize(uIlluminationSampler, 0).x);

    for (int x = -FILTER_RADIUS; x <= FILTER_RADIUS; x++) {
        for (int y = -FILTER_RADIUS; y <= FILTER_RADIUS; y++) {
            if (x == 0 && y == 0) {
                continue;
            }

            vec2 pos = vTexture + vec2(x, y) * illumTexelSize * STEP_SIZE;

            vec4 currNormal = texture(uNormalSampler, pos);
            float currDepth = currNormal.w;

            float weightNormal = normalEdgeStoppingWeight(centerNormal.xyz, currNormal.xyz, 128.0);
            float weightDepth = depthEdgeStoppingWeight(centerDepth, currDepth, 0.01);

            
            illumSum += texture(uIlluminationSampler, pos).rgb * weightNormal * weightDepth;
            weightSum += weightNormal * weightDepth;
        }
    }

    illumSum /= weightSum;

    vec4 color = texture(uAlbedoSampler, vTexture);

    fColor = vec4(color.rgb * illumSum, 1.0f);

    fPrevIllumination = texture(uIlluminationSampler, vTexture);

    gl_FragDepth = color.w;  
}