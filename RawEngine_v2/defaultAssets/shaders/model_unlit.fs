#version 400 core
out vec4 FragColor;
in vec3 fPos;
in vec3 fNor;
in vec2 uv;

struct Material
{
   sampler2D textureMap;
   sampler2D normalMap;
   sampler2D specularMap;
   vec4 color;
   float shininess;
};
uniform Material material;

void main()
{
   vec4 diffuseColor = texture(material.textureMap, uv) + vec4(material.color.a * vec3(material.color.rgb), 0.0f);
   FragColor = diffuseColor;
}