#version 450
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 Normal;
layout(location = 1) in vec4 WorldPos;
layout(location = 2) in vec2 TexCoords;
layout(set = 1, binding = 0) uniform sampler2D diffuse;

void main()
{
    vec4 diff = texture(diffuse, TexCoords);
    FragColor = diff;
}