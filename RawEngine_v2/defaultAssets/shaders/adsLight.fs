#version 400 core
out vec4 FragColor;
in vec3 fPos;
in vec3 fTan;
in vec3 fNor;
in vec3 fBit;
in vec2 uv;
in mat3 tbn;

struct Material
{
    sampler2D textureMap;
    sampler2D normalMap;
    sampler2D specularMap;
};
uniform Material material;

uniform float ambientIntensity;
uniform vec4 ambientColor;
uniform int lightCount;
uniform vec3 cameraPosition;

struct Light
{
    vec4 color;
    vec3 position;
};
uniform Light lights[20];

vec4 diffuseColor;
vec3 normal;
vec4 Ambient;
vec4 Diffuse;
vec4 Attenuation;
vec4 Specular;
float specularIntensity;

void main()
{
    FragColor = vec4(0.0);
    diffuseColor = texture(material.textureMap, uv);
    normal = texture(material.normalMap, uv).rgb;
    normal = normal * 2.0 - 1.0;
    specularIntensity = texture(material.specularMap, uv).r;

    diffuseColor.a = 1;
    Ambient = ambientIntensity * ambientColor * diffuseColor;
    Ambient.a = 1;
    FragColor += Ambient;
    for (int i = 0; i < lightCount; i++)
    {
        vec3 direction = normalize(lights[i].position - fPos);
        Diffuse = max(dot(tbn * direction, normal), 0) * lights[i].color * diffuseColor;
        Attenuation = Diffuse / (1.0 + 0.7 + 1.8);
        vec3 reflection = reflect(normal, direction);
        vec3 camDirection = normalize(fPos - cameraPosition);

        Specular = pow(max(dot(camDirection, reflection), 0), specularIntensity) * lights[i].color * diffuseColor;
        FragColor += Ambient + Attenuation + Specular;
    }
}