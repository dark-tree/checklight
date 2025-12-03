#version 450
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "scene.glsl"

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec2 vTexture;
layout(location = 2) in flat uint vMaterialIndex;
layout(location = 3) in vec3 vWorldPos;

layout(location = 0) out vec4 fColor;

struct RenderMaterial {
    uint index;
    vec4 albedo;
    vec3 emissive;
    vec3 specular;
    float shininess;
    uint albedoTextureIndex;
};

struct Light {
    uint type;
    vec3 vector;
    vec3 color;
    float intensity;
    bool shadow;
};

layout(binding = 1, set = 0, scalar) uniform _SceneUniform { SceneUniform uSceneObject; };
layout(binding = 3, set = 0) uniform sampler2D textures[];
layout(binding = 4, set = 0, scalar) readonly buffer MaterialBuffer { RenderMaterial i[]; } materials;
layout(binding = 5, set = 0, scalar) readonly buffer LightBuffer { Light i[]; } lights;

vec3 lightdir = vec3(1,1,1);
//temporary
float shininess = 100;
float ambient = 0.2;
vec3 shadowLight = vec3(0.06,0.06,0.1);
vec3 lightColor = vec3(1,0.98,0.95);

void main() {
    vec3 fragPos = vWorldPos;

    RenderMaterial mat = materials.i[vMaterialIndex];

    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightdir);
    vec3 V = normalize((uSceneObject.viewInv * vec4(0.0, 0.0, -1.0, 0.0)).xyz);
    vec3 R = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(V, R), 0.0), shininess);

    vec4 baseColor;
    if (mat.albedoTextureIndex != 0) {
        baseColor = texture(textures[nonuniformEXT(mat.albedoTextureIndex)], vTexture);
    } else {
        baseColor = vec4(1,1,1,1);
    }

    vec3 lighting = baseColor.rgb * (diff + ambient) * lightColor;
    vec3 shadow = baseColor.rgb * shadowLight * (1 + ambient);


    vec3 totalPointLight = vec3(0.0);

    for(int i = 0; i < lights.i.length(); i++){
        Light light = lights.i[i];
        float dist = distance(fragPos,light.vector);

        vec3 lightDir = normalize(light.vector - fragPos);

        //to match raytracing brightness EMPIRICAL!!
        float intensity = light.intensity * 0.15;
        if(dist < 20 * (intensity + 0.3)){

            //brightness falloff
            float a = 0.09;
            float b = 0.032;
            float attenuation = 1.0 / (1 + a * dist + b * (dist * dist));

            float pointDiff = max(dot(N,lightDir), 0.0);

            vec3 diffuseLight = pointDiff * baseColor.rgb * light.color;
            vec3 pReflect = reflect(-lightDir,R);
            float pSpec = pow(max(dot(V,pReflect), 0.0), shininess);

            totalPointLight += (diffuseLight + mat.specular * pSpec * 0.5) * attenuation * intensity;
        }
    }
    vec3 final = mix(shadow,lighting, diff) + mat.specular * spec * 0.5 + mat.emissive;

    fColor = vec4(final + totalPointLight, 1.0);
}