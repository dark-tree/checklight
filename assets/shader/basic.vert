#version 450

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iColor;

layout(binding = 0) uniform SceneUniform {
    mat4 view;
    mat4 projection;
} uSceneObject;

layout(push_constant) uniform ChunkUniform {
    mat4 model;
    int material;
} uMeshObject;

layout(location = 0) out vec4 vColor;

void main() {
    mat4 matrix = uSceneObject.projection * uSceneObject.view * uMeshObject.model;

    gl_Position = matrix * vec4(iPosition, 1.0);
    vColor = vec4(iColor, 1.0f);
}