#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform sampler2D equirectangularMap;
const float PI = 3.1415926;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap2(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), acos(v.y));
    //uv *= invAtan;
    uv.x += PI;
    uv *= vec2(1/(2*PI), 1/PI);
    uv.y = 1 - uv.y;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(WorldPos));
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    
    FragColor = vec4(color, 1.0);
}