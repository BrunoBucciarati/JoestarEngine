#version 450

layout(location = 0) in vec3 TexCoords;
layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform samplerCube skybox;

void main()
{
    outColor = vec4(textureLod(skybox, TexCoords, 0.0));
    // outColor = vec4(1.0, 0.0, 0.0, 1.0);
}