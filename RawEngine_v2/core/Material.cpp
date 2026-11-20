#include "Material.h"

#include <fstream>
#include <sstream>

namespace core
{

    std::string readFileToString(const std::string &filePath)
    {
        std::ifstream fileStream(filePath, std::ios::in);
        if (!fileStream.is_open())
        {
            printf("Could not open file: %s\n", filePath.c_str());
            return "";
        }
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        return buffer.str();
    }


    GLuint generateShader(const std::string &shaderPath, GLuint shaderType)
    {
        printf("Loading shader: %s\n", shaderPath.c_str());
        const std::string shaderText = readFileToString(shaderPath);
        const GLuint shader = glCreateShader(shaderType);
        const char *s_str = shaderText.c_str();
        glShaderSource(shader, 1, &s_str, nullptr);
        glCompileShader(shader);
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            printf("Error! Shader issue [%s]: %s\n", shaderPath.c_str(), infoLog);
        }
        return shader;
    }

    Material::Material()
    {
        shaderProgram = 0;
    }

    GLuint Material::GetVertexShader()
    {
        return vertexShader;
    }
    GLuint Material::GetFragmentShader()
    {
        return fragmentShader;
    }

    std::string Material::GetVertexShaderPath()
    {
        return vsPath;
    }

    std::string Material::GetFragmentShaderPath()
    {
        return fsPath;
    }

    void Material::SetVertexShader(std::string path)
    {
        vertexShader = generateShader(path, GL_VERTEX_SHADER);;
        vsPath = path;
        shaderProgram = 0;
        printf("Set vs: %s\n", path.c_str());
    }
    void Material::SetFragmentShader(std::string path)
    {
        fragmentShader = generateShader(path, GL_FRAGMENT_SHADER);
        fsPath = path;
        shaderProgram = 0;
        printf("Set fs: %s\n", path.c_str());
    }

    unsigned int Material::CreateShaderProgram()
    {
        int success;
        char infoLog[512];
        unsigned int shaderProgram = glCreateProgram();
        if (vertexShader == 0) SetVertexShader("defaultAssets/shaders/model_default.vs");
        glAttachShader(shaderProgram, vertexShader);
        if (fragmentShader == 0) SetFragmentShader("defaultAssets/shaders/model_lit_ads.fs");
        glAttachShader(shaderProgram, fragmentShader);
        printf("Created ShaderProgram with %s and %s.\n", GetVertexShaderPath().c_str(), GetFragmentShaderPath().c_str());

        glLinkProgram(shaderProgram);
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            printf("Error! Making Shader Program: %s\n", infoLog);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        std::string materialUniform = "material.";

        GLint materialTextureMapUniform = glGetUniformLocation(shaderProgram, (materialUniform + "textureMap").c_str());
        GLint materialNormalMapUniform = glGetUniformLocation(shaderProgram, (materialUniform + "normalMap").c_str());
        GLint materialSpecularMapUniform = glGetUniformLocation(shaderProgram, (materialUniform + "specularMap").c_str());
        GLint materialColorUniform = glGetUniformLocation(shaderProgram, (materialUniform + "color").c_str());
        GLint materialShininessUniform = glGetUniformLocation(shaderProgram, (materialUniform + "shininess").c_str());

        Texture& diffuse = GetTextureMap();
        Texture& normal = GetNormalMap();
        Texture& specular = GetSpecularMap();

        glUseProgram(shaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(materialTextureMapUniform, 0);
        glBindTexture(GL_TEXTURE_2D, diffuse.getId());
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(materialNormalMapUniform, 1);
        glBindTexture(GL_TEXTURE_2D, normal.getId());
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(materialSpecularMapUniform, 2);
        glBindTexture(GL_TEXTURE_2D, specular.getId());

        glUniform4f(materialColorUniform, color.r, color.g, color.b, color.a);
        glUniform1f(materialShininessUniform, specularStrength);
        glBindVertexArray(0);

        return shaderProgram;
    }

    void Material::DeleteShaderProgram()
    {
        glDeleteProgram(shaderProgram);
    }

    unsigned int Material::GetShaderProgram()
    {
        if (shaderProgram == 0)
        {
            shaderProgram = CreateShaderProgram();
        }

        glUseProgram(shaderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureMap.getId());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap.getId());

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, specularMap.getId());

        std::string materialUniform = "material.";
        GLint materialColorUniform = glGetUniformLocation(shaderProgram, (materialUniform + "color").c_str());
        glUniform4f(materialColorUniform, color.r, color.g, color.b, color.a);
        GLint materialShininessUniform = glGetUniformLocation(shaderProgram, (materialUniform + "shininess").c_str());
        glUniform1f(materialShininessUniform, specularStrength);

        return shaderProgram;
    }

    core::Texture& Material::GetTextureMap()
    {
        return textureMap;
    }
    core::Texture& Material::GetNormalMap()
    {
        return normalMap;
    }
    core::Texture& Material::GetSpecularMap()
    {
        return specularMap;
    }

    void Material::SetTextureMap(std::string path)
    {
        this->textureMap = Texture(path);
    }
    void Material::SetNormalMap(std::string path)
    {
        this->normalMap = Texture(path);
    }
    void Material::SetSpecularMap(std::string path)
    {
        this->specularMap = Texture(path);
    }

    float Material::GetSpecularStrength()
    {
        return specularStrength;
    }
    void Material::SetSpecularStrength(float specularStrength)
    {
        this->specularStrength = specularStrength;
    }

    glm::vec4 Material::GetColor()
    {
        return color;
    }
    void Material::SetColor(glm::vec4 color)
    {
        this->color = color;
    }

}
