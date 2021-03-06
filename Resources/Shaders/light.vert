#version 450

layout(push_constant) uniform PushConsts {		//声明接受数据部分
	mat4 model;
} pushConsts;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 instancePos;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * pushConsts.model * vec4(inPosition + instancePos, 1.0);
    fragColor = inColor;
}