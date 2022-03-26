#version 450
out vec4 FragColor;

layout(location = 0) in vec3 Normal;
layout(location = 1)in vec4 WorldPos;
layout(location = 2)in vec2 TexCoords;

void main()
{
    FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}