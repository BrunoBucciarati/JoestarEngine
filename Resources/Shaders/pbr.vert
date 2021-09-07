#version 450
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inColor;
layout (location = 2) in vec3 inNormal;

layout(location = 0) out vec2 TexCoords;
layout(location = 1) out vec3 WorldPos;
layout(location = 2) out vec3 Normal;

layout(binding = 0) uniform UniformBufferObject {
    //mat4 model;
    mat4 view;
    mat4 proj;
    vec3 camPos;
} ubo;

layout(push_constant) uniform PushConsts {
	mat4 model;
} pushConsts;

void main()
{
    TexCoords = aTexCoords;
    WorldPos = vec3(pushConsts.model * vec4(aPos, 1.0));
    Normal = mat3(pushConsts.model) * aNormal;   

    gl_Position =  ubo.projection * ubo.view * vec4(WorldPos, 1.0);
}