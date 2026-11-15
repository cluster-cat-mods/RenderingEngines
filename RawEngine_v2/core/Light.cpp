#include "Light.h"

namespace core
{
    Light::Light(core::Transform transform, glm::vec4 color)
    {
        this->transform = transform;
        this->color = color;
    }

    glm::vec4 Light::GetColor()
    {
        return this->color;
    }
    void Light::SetColor(glm::vec4 color)
    {
        this->color = color;
    }
}