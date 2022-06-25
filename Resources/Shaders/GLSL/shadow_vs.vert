#version 450
layout (location = 0) in vec3 aPosition;

layout (std140, set = 0, binding = 0) uniform Pass
{
    mat4 view;
    mat4 proj;
};
layout(std140, set = 2, binding = 0) uniform Object
{
    mat4 model;
};
void main()
{
   gl_Position = proj * view * model * vec4(aPosition, 1.0);
}