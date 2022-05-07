#version 450
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord0;

layout(location = 0) out vec3 Normal;
layout(location = 1) out vec4 WorldPos;
layout(location = 2) out vec2 TexCoords;
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
   WorldPos = model * vec4(aPosition, 1.0);
   Normal = (model * vec4(aNormal, 1.0)).xyz;
   TexCoords = aTexCoord0;
}