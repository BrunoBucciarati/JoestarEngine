#version 330 core
out vec4 FragColor;
// in vec4 ourColor;
in vec2 uv;
uniform sampler2D ourTexture;
void main()
{
    vec4 rgba = texture(ourTexture, uv);
    FragColor = vec4(rgba.x + 1.0, 1.0, 1.0, 1.0);
    //FragColor = vec4(1.0);
}