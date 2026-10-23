#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>
#include <cmath>

namespace core
{
    Transform::Transform(glm::vec3 position, glm::vec3 rotation  = glm::vec3(0.0f), glm::vec3 scale  = glm::vec3(0.0f))
    {
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale.x, scale.y, scale.z));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::mat4(1);
    }

    glm::vec3 Transform::GetPosition()
    {
        return glm::vec3(modelMatrix[3]);
    }
    /*
    glm::vec3 Transform::rotation()
    {
        //
    }
    glm::vec3 Transform::scale()
    {
        glm::vec3 scale;
        scale.x = std::sqrt(modelMatrix[0][0]*modelMatrix[0][0] + modelMatrix[0][1]*modelMatrix[0][1] + modelMatrix[0][2]*modelMatrix[0][2]);
        scale.y = std::sqrt(modelMatrix[1][0]*modelMatrix[1][0] + modelMatrix[1][1]*modelMatrix[1][1] + modelMatrix[1][2]*modelMatrix[1][2]);
        scale.z = std::sqrt(modelMatrix[2][0]*modelMatrix[2][0] + modelMatrix[2][1]*modelMatrix[2][1] + modelMatrix[2][2]*modelMatrix[2][2]);
        //
    }
    */

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
