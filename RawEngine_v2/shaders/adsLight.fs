#version 400 core
out vec4 FragColor;
in vec3 fPos;
in vec3 fNor;
in vec2 uv;
uniform sampler2D text;
uniform sampler2D normalMap;

uniform float ambientIntensity;
uniform vec4 ambientColour;
uniform int lightCount;

struct Light
{
    vec4 colour;
    vec3 position;
};
uniform Light lights[20];

vec4 diffuseColour;
vec4 Ambient;
vec4 Diffuse;

void main()
{
    FragColor = vec4(0.0);
    diffuseColour = texture(text, uv);
    Ambient = ambientIntensity * ambientColour * diffuseColour;
    Ambient.a = 1;
    FragColor += Ambient;
    for (int i = 0; i < lightCount; i++)
    {
        Diffuse = max(dot(normalize(lights[i].position - fPos), fNor), 0) * lights[i].colour * diffuseColour;
        FragColor += Ambient + Diffuse;
    }

}