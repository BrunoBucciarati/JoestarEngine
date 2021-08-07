#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform    mat4 projection;
uniform    mat4 view;
void main()
{
    TexCoords = aPos;
    mat4 view1 = view;
    view1[3].xyz = vec3(0.0);
    gl_Position = projection * view1 * vec4(aPos, 1.0);
    gl_Position.w = gl_Position.z + 0.01;
}