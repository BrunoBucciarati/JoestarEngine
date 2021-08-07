#version 330 core
out vec4 FragColor;
// in vec4 ourColor;
// in vec2 uv;
in vec3 Normal;
in vec4 WorldPos;
in vec2 TexCoords;
//uniform sampler2D ourTexture;
// struct Material {
//     sampler2D diffuse;
//     vec3      specular;
//     float     shininess;
// }; 

// uniform Material material;
uniform sampler2D texture1;
uniform samplerCube skyboxTexture;
const vec3 lightColor = vec3(1.2, 1.2, 1.2);
uniform vec3 lightDir;
uniform vec3 viewPos;
void main()
{
    float ambientStength = 0.3;
    vec4 albedo = texture(texture1, TexCoords);
    vec3 ambient = lightColor * albedo.xyz * ambientStength;
    vec3 dir = lightDir;//normalize(lightPos - WorldPos.xyz);
    float ndl = dot(dir, Normal);
    vec3 diffuse = max(ndl, 0.0) * lightColor * albedo.xyz * 0.2;

    vec3 reflectDir = reflect(-dir, Normal);
    vec3 viewDir = normalize(viewPos - WorldPos.xyz);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.5 * spec * lightColor;

    FragColor.xyz = diffuse + specular + ambient;
    // FragColor.xyz = vec3(1.0);
    FragColor.w = 1.0;
    // FragColor = albedo;
    // float ratio = 1.00 / 1.52;
    // vec3 R = refract(-viewDir, normalize(Normal), ratio);
    // FragColor = vec4(texture(skyboxTexture, R).rgb, 1.0);
}