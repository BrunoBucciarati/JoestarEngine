#version 450
struct SH9 {
    float coefs[9];
};

SH9 ProjectOntoSH9(vec3 dir, uint level)
{
    SH9 sh;
    // Band 0
    sh.coefs[0] = 0.282095f;

    // Band 1
    sh.coefs[1] = 0.488603f * dir.y;
    sh.coefs[2] = 0.488603f * dir.z;
    sh.coefs[3] = 0.488603f * dir.x;

    // Band 2
    if (level > 1) {
        sh.coefs[4] = 1.092548f * dir.x * dir.y;
        sh.coefs[5] = 1.092548f * dir.y * dir.z;
        sh.coefs[6] = 0.315392f * (3.0f * dir.z * dir.z - 1.0f);
        sh.coefs[7] = 1.092548f * dir.x * dir.z;
        sh.coefs[8] = 0.546274f * (dir.x * dir.x - dir.y * dir.y);
    }

    return sh;
}
//
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 TexCoords;
layout(location = 1) in vec3 WorldPos;
layout(location = 2) in vec3 Normal;


layout(binding = 1) uniform UniformBufferObjectPS {
    vec4 camPos;
    vec4 sunDirection;
    vec4 sunColor;
} ubo;

const int MAX_LIGHTS = 20;
layout(binding = 2, std140) uniform LightBlocks {
    vec4 lightPositions[MAX_LIGHTS];
    vec4 lightColors[MAX_LIGHTS];
    uint lightCount;
    vec3 shCoefs[9];
} lightBlocks;
// material parameters
layout(binding = 3) uniform sampler2D albedoMap;
layout(binding = 4) uniform sampler2D normalMap;
layout(binding = 5) uniform sampler2D metallicMap;
layout(binding = 6) uniform sampler2D roughnessMap;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 GetColor(vec3 radiance, vec3 N, vec3 V, vec3 L, vec3 H, float roughness, vec3 F0, float metallic, vec3 albedo) {
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  
        
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        return (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}

vec3 GetSHDiffuse(vec3 normal) {

}

// ----------------------------------------------------------------------------
void main()
{		
    vec3 albedo     = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, TexCoords).r;
    float roughness = texture(roughnessMap, TexCoords).r;
    float ao        = 1.0;//texture(aoMap, TexCoords).r;

    vec3 N = Normal;
    vec3 V = normalize(ubo.camPos.xyz - WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    vec3 L = -ubo.sunDirection.xyz;
    vec3 H = normalize(V + L);
    // reflectance equation
    vec3 Lo = vec3(0.0);
    //main Light
    Lo += GetColor(ubo.sunColor.xyz, N, V, L, H, roughness, F0, metallic, albedo);


    // for(int i = 0; i < 4; ++i) 
    for(int i = 0; i < lightBlocks.lightCount; ++i) 
    {
        // calculate per-light radiance
        L = normalize(lightBlocks.lightPositions[i].xyz - WorldPos);
        H = normalize(V + L);
        float dis = length(lightBlocks.lightPositions[i].xyz - WorldPos);
        float attenuation = 1.0 / (dis * dis);
        vec3 radiance = lightBlocks.lightColors[i].rgb * attenuation;
        Lo += GetColor(radiance, N, V, L, H, roughness, F0, metallic, albedo);
    }   
    vec3 color = Lo;
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}