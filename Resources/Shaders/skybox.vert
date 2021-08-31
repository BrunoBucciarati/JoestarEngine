#version 450

layout(binding = 0) uniform UniformBufferObject {
    //mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 TexCoords;

void main()
{
    TexCoords = inPosition;
    mat4 view1 = ubo.view;
    view1[3].xyz = vec3(0.0);
    gl_Position = ubo.proj * view1 * vec4(inPosition, 1.0);
    gl_Position.w = gl_Position.z;
}