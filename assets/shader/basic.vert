#version 450

layout(location = 0) in vec2 iPosition;
layout(location = 1) in vec3 iColor;

layout(binding = 0) uniform SceneUniform {
    mat4 model;
    mat4 view_projection;
} uSceneObject;

layout(location = 0) out vec4 vColor;

void main() {
    mat4 matrix = uSceneObject.view_projection;

    gl_Position = matrix * vec4(iPosition, 1.0, 1.0);
    vColor = vec4(iColor, 1.0f);
}