#version 450
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec4 vColor;
layout(location = 1) in vec2 vTexture;
layout(location = 2) in flat uint vMaterialIndex;

layout(location = 0) out vec4 fColor;

struct RenderMaterial {
    uint index;
    vec4 albedo;
    vec3 emissive;
    vec3 specular;
    float shininess;
    uint albedoTextureIndex;
};

layout(binding = 3, set = 0) uniform sampler2D textures[];
layout(binding = 4, set = 0, scalar) readonly buffer MaterialBuffer { RenderMaterial i[]; } materials;


void main() {
    RenderMaterial mat = materials.i[vMaterialIndex];
    vec4 baseColor = mat.albedo * vColor;

    if (mat.albedoTextureIndex != 0) {
        fColor = texture(textures[nonuniformEXT(mat.albedoTextureIndex)], vTexture);
    }
    else{
        fColor=vColor;
    }
}