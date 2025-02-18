#version 450

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec4 iColor;
layout(location = 2) in vec2 iTexture;

//layout(binding = 0) uniform SceneUniform {
//    mat4 view;
//    mat4 projection;
//} uSceneObject;

layout(location = 0) out vec4 vColor;
layout(location = 1) out vec2 vTexture;

void main() {
    //mat4 matrix = uSceneObject.projection * uSceneObject.view;

    gl_Position = vec4(iPosition, 1.0);
    vColor = iColor;
    vTexture = iTexture;
}