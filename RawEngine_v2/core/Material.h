#pragma once
#include <vector>
#include <glm/vec4.hpp>

#include "texture.h"

namespace core
{
    class Material
    {
    private:
        GLuint vertexShader = 0;
        GLuint fragmentShader = 0;

        std::string vsPath;
        std::string fsPath;

        unsigned int shaderProgram = 0;

        Texture textureMap = Texture(glm::vec3(1.0f));
        Texture normalMap = Texture(glm::vec3(0.5f, 0.5f, 1.0f));
        Texture specularMap = Texture(glm::vec3(0.0f));

        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 0.1f);
        float specularStrength = 0.0f;


    public:
        Material();

        GLuint GetVertexShader();
        GLuint GetFragmentShader();

        void SetVertexShader(std::string path);
        void SetFragmentShader(std::string path);

        std::string GetVertexShaderPath();
        std::string GetFragmentShaderPath();

        unsigned int CreateShaderProgram();
        void DeleteShaderProgram();
        unsigned int GetShaderProgram();

        core::Texture& GetTextureMap();
        core::Texture& GetNormalMap();
        core::Texture& GetSpecularMap();

        void SetTextureMap(std::string path);
        void SetNormalMap(std::string path);
        void SetSpecularMap(std::string path);

        float GetSpecularStrength();
        void SetSpecularStrength(float specularStrength);

        glm::vec4 GetColor();
        void SetColor(glm::vec4 color);

    };
}
