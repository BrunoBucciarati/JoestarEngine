#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aOffset;
out vec3 Normal;
out vec4 WorldPos;
out vec2 TexCoords;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;
void main()
{
   gl_Position = projection * view * model * vec4(aPos + aOffset, 1.0);
   WorldPos = model * vec4(aPos, 1.0);
//    ourColor = vec4(aColor, 1.0);
   Normal = (model * vec4(aNormal, 1.0)).xyz;
   //Normal = mat3(transpose(inverse(model))) * aNormal;
   TexCoords = aTexCoords;
}