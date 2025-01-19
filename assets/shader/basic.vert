#version 450

layout(location = 0) in vec2 iPosition;
layout(location = 1) in vec3 iColor;

layout(location = 0) out vec4 vColor;

void main() {
    gl_Position = vec4(iPosition, 0.0, 1.0);
    vColor = vec4(iColor, 1.0f);
}