#pragma once
#include <vector>
#include <glm/vec4.hpp>

#include "texture.h"

namespace core
{
    class Material
    {
    private:
        std::vector<GLuint> vertexShaders;
        std::vector<GLuint> fragmentShaders;

        unsigned int shaderProgram = 0;

        Texture textureMap;
        Texture normalMap;
        Texture specularMap;




    public:
        Material();

        GLuint AddVertexShader(std::string path);
        GLuint AddFragmentShader(std::string path);

        unsigned int CreateShaderProgram();
        void DeleteShaderProgram();
        unsigned int GetShaderProgram();

        core::Texture& GetTextureMap();
        core::Texture& GetNormalMap();
        core::Texture& GetSpecularMap();

        void SetTextureMap(std::string path);
        void SetNormalMap(std::string path);
        void SetSpecularMap(std::string path);

    };
}
