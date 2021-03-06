#version 450

layout(location = 0) in vec3 TexCoords;
layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform samplerCube skybox;

void main()
{    
    outColor = texture(skybox, TexCoords);
}