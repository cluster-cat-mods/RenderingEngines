#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/vec3.hpp>

namespace core {

    class Texture {
    private:
        GLuint id;

    public:
        Texture(const std::string& path);
        Texture(glm::vec3 color);

        GLuint getId();
    };

}
