#include "Transform.h"

namespace core
{
    Transform::Transform(glm::vec3 position, glm::vec3 rotation  = glm::vec3(0.0f), glm::vec3 scale  = glm::vec3(0.0f))
    {
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
    }
}