#include "Light.h"

namespace core
{
    Light::Light(core::Transform transform, glm::vec4 colour)
    {
        this->transform = transform;
        this->colour = colour;
    }

    glm::vec4 Light::GetColour()
    {
        return this->colour;
    }
    void Light::SetColour(glm::vec4 colour)
    {
        this->colour = colour;
    }
}