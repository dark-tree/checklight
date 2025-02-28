#version 450
// See: https://github.com/SaschaWillems/Vulkan/blob/master/shaders/glsl/ssao/fullscreen.vert
// maybe split into two triangles with no off-screen parts (?)

layout(location = 0) out vec2 vTexture;

void main() {
    vTexture = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(vTexture * 2.0f - 1.0f, 1.0f, 1.0f);
}