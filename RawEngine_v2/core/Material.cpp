#include "Material.h"

namespace core
{
    core::Texture Material::GetDiffuseTexture()
    {
        return diffuseTexture;
    }
    core::Texture Material::GetNormalTexture()
    {
        return normalTexture;
    }
    core::Texture Material::GetSpecularTexture()
    {
        return specularTexture;
    }

    void Material::SetDiffuseTexture(std::string path)
    {
        this->diffuseTexture = Texture(path);
    }
    void Material::SetNormalTexture(std::string path)
    {
        this->normalTexture = Texture(path);
    }
    void Material::SetSpecularTexture(std::string path)
    {
        this->specularTexture = Texture(path);
    }
}