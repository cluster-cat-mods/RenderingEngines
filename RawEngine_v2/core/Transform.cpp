#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cmath>
#include <iostream>
#include <ostream>

namespace core
{
    Transform::Transform(glm::vec3 position, glm::vec3 rotation  = glm::vec3(0.0f), glm::vec3 scale  = glm::vec3(1.0f))
    {
        modelMatrix = glm::mat4(1.0f);
        SetPosition(position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale.x, scale.y, scale.z));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 Transform::GetPosition()
    {
        return glm::vec3(modelMatrix[3]);
    }

    glm::vec3 Transform::GetRotation()
    {
        glm::quat rotationQuat = glm::quat_cast(modelMatrix);
        return glm::degrees(glm::eulerAngles(rotationQuat));
    }

    glm::vec3 Transform::GetScale()
    {
        glm::vec3 scale;
        scale.x = std::sqrt(modelMatrix[0][0]*modelMatrix[0][0] + modelMatrix[0][1]*modelMatrix[0][1] + modelMatrix[0][2]*modelMatrix[0][2]);
        scale.y = std::sqrt(modelMatrix[1][0]*modelMatrix[1][0] + modelMatrix[1][1]*modelMatrix[1][1] + modelMatrix[1][2]*modelMatrix[1][2]);
        scale.z = std::sqrt(modelMatrix[2][0]*modelMatrix[2][0] + modelMatrix[2][1]*modelMatrix[2][1] + modelMatrix[2][2]*modelMatrix[2][2]);
        return scale;
    }

    void Transform::SetPosition(glm::vec3 position)
    {
        modelMatrix[3][0] = position.x;
        modelMatrix[3][1] = position.y;
        modelMatrix[3][2] = position.z;
    }

    void Transform::SetRotation(glm::vec3 rotation)
    {
        glm::vec3 position = GetPosition();
        glm::vec3 scale = GetScale();

        modelMatrix = glm::mat4(1.0f);
        SetPosition(position);
        Scale(scale);
        Rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(rotation.z));
        Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(rotation.y));
        Rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(rotation.x));
    }


    void Transform::Translate(glm::vec3 translation)
    {
        modelMatrix = glm::translate(modelMatrix, translation);
    }
    void Transform::Rotate(glm::vec3 axis, float radians)
    {
        modelMatrix = glm::rotate(modelMatrix, radians, axis);
    }
    void Transform::Scale(glm::vec3 scale)
    {
        modelMatrix = glm::scale(modelMatrix, scale);
    }

    glm::mat4 Transform::GetModelMatrix() const {
        return this->modelMatrix;
    }
}
