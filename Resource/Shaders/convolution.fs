#version 330 core
out vec4 FragColor;
in vec3 WorldPos;
uniform samplerCube envMap;
const float PI = 3.1415926;
vec3 Convolution(vec3 normal)
{
    vec3 irradiance = vec3(0.0);  

    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up        = cross(normal, right);

    float sampleDelta = 0.25;
    float nrSamples = 0.0; 
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

            // irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            irradiance += texture(envMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    return irradiance;
}

void main()
{   
    vec3 irradiance = Convolution(WorldPos);
    
    FragColor = vec4(irradiance, 1.0);
}
