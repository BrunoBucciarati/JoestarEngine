#version 450

layout (std140, binding = 0, set = 0) uniform Pass
{
    mat4 view;
    mat4 proj;
};

layout(location = 0) in vec3 aPosition;
layout(location = 0) out vec3 aTexCoords;

void main()
{
    aTexCoords = aPosition;
    aTexCoords.y = -aTexCoords.y;
    mat4 view1 = view;
    view1[3].xyz = vec3(0.0);
    gl_Position = proj * view1 * vec4(aPosition * 1.0, 1.0);
    gl_Position.w = gl_Position.z;
}