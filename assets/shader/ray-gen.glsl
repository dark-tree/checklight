#version 460
#extension GL_EXT_ray_tracing : require

struct hitPayload
{
  vec3 hitValue;
};

layout(location = 0) rayPayloadEXT hitPayload prd;

layout(binding = 0, set = 0) uniform accelerationStructureEXT TLAS;
layout(binding = 1, set = 0, rgba32f) uniform image2D image;

layout(binding = 2) uniform SceneUniform {
    mat4 view;
    mat4 projection;
} uSceneObject;

void main() {
    const vec2 pixelCenter = vec2(gl_LaunchIDEXT.xy) + vec2(0.5);
    const vec2 inUV = pixelCenter/vec2(gl_LaunchSizeEXT.xy);
    vec2 d = inUV * 2.0 - 1.0;

    mat4 viewInverse = inverse(uSceneObject.view);
    mat4 projInverse = inverse(uSceneObject.projection);

    vec4 origin = viewInverse * vec4(0, 0, 0, 1);
    vec4 target = projInverse * vec4(d.x, d.y, 1, 1);
    vec4 direction = viewInverse * vec4(normalize(target.xyz), 0);

    uint  rayFlags = gl_RayFlagsOpaqueEXT;
    float tMin = 0.001;
    float tMax = 10000.0;

    traceRayEXT(TLAS, // acceleration structure
          rayFlags,       // rayFlags
          0xFF,           // cullMask
          0,              // sbtRecordOffset
          0,              // sbtRecordStride
          0,              // missIndex
          origin.xyz,     // ray origin
          tMin,           // ray min range
          direction.xyz,  // ray direction
          tMax,           // ray max range
          0               // payload (location = 0)
    );

    imageStore(image, ivec2(gl_LaunchIDEXT.xy), vec4(prd.hitValue, 1.0));
}
