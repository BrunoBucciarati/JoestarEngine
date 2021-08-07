#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec4 WorldPos;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 lightDir;
const vec3 lightColor = vec3(1, 1, 1);
uniform vec3 viewPos;
uniform mat4 shadowMatrix;
uniform sampler2D shadowMap;
void main()
{
    float ambientStength = 0.3;
    vec4 albedo = texture(texture1, TexCoords);
    vec3 ambient = lightColor * albedo.xyz * ambientStength;
    vec3 dir = -lightDir;//normalize(lightPos - WorldPos.xyz);
    float ndl = dot(dir, Normal);
    vec3 diffuse = max(ndl, 0.0) * lightColor * albedo.xyz * 0.2;

    vec3 reflectDir = reflect(-dir, Normal);
    vec3 viewDir = normalize(viewPos - WorldPos.xyz);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.5 * spec * lightColor;

    vec4 shadowPos = shadowMatrix * WorldPos;
    vec3 shadowuvz = shadowPos.xyz / shadowPos.w;
    shadowuvz = shadowuvz * 0.5 + 0.5;
    float depth = texture(shadowMap, shadowuvz.xy).r;

    //FragColor.xyz = (diffuse + specular + ambient) * step(shadowuvz.z, depth);
    FragColor.xyz = ambient + (diffuse) * step(shadowuvz.z, depth);
    //FragColor.xyz = albedo;
    // FragColor.xyz = vec3(1.0);
    FragColor.w = 1.0;
    // float ratio = 1.00 / 1.52;
    // vec3 R = refract(-viewDir, normalize(Normal), ratio);
    // FragColor = vec4(texture(skyboxTexture, R).rgb, 1.0);
}