#version 330 core
out vec4 FragColor;
// in vec4 ourColor;
// in vec2 uv;
in vec3 normal;
in vec4 WorldPos;
in vec2 TexCoords;
//uniform sampler2D ourTexture;
struct Material {
    sampler2D diffuse;
    vec3      specular;
    float     shininess;
}; 

uniform Material material;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
void main()
{
    float ambientStength = 0.1;
    vec3 albedo = texture(material.diffuse, TexCoords).xyz;
    vec3 ambient = lightColor * albedo * ambientStength;
    vec3 dir = normalize(lightPos - WorldPos.xyz);
    float ndl = dot(dir, normal);
    vec3 diffuse = max(ndl, 0.0) * lightColor * albedo;

    vec3 reflectDir = reflect(-dir, normal);
    vec3 viewDir = normalize(viewPos - WorldPos.xyz);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = material.specular * spec * lightColor;

    FragColor.xyz = diffuse + specular + ambient;
    // FragColor.xyz = vec3(1.0);
    FragColor.w = 1.0;
}