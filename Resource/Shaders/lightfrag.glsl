#version 330 core
out vec4 FragColor;
// in vec4 ourColor;
in vec2 uv;
uniform sampler2D ourTexture;
void main()
{
    vec4 rgba = texture(ourTexture, uv);
    FragColor = vec4(rgba.xyz, 1.0);
}