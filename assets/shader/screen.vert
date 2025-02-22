#version 450

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec4 iColor;
layout(location = 2) in vec2 iTexture;

layout(location = 0) out vec4 vColor;
layout(location = 1) out vec2 vTexture;

layout(push_constant) uniform MeshConstant {
    mat4 matrix;
} uMeshObject;

void main() {
    gl_Position = uMeshObject.matrix * vec4(iPosition, 1.0);
    vColor = iColor;
    vTexture = iTexture;
}