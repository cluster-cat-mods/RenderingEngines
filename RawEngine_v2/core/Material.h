#pragma once
#include "texture.h"

namespace core
{
    class Material
    {
    private:
        core::Texture diffuseTexture;
        core::Texture normalTexture;
        core::Texture specularTexture;

        


    public:
        Material();

        core::Texture GetDiffuseTexture();
        core::Texture GetNormalTexture();
        core::Texture GetSpecularTexture();

        void SetDiffuseTexture(std::string path);
        void SetNormalTexture(std::string path);
        void SetSpecularTexture(std::string path);

    };
}
