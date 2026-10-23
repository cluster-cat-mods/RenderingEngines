#pragma once
#include <glm/vec3.hpp>

namespace core
{
    class Scene
    {
    private:
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    public:
        glm::vec3 WorldUp() const
        {
            return worldUp;
        }


    };
}

