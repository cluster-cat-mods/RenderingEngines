#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace core
{
    class Transform
    {
    private:
        glm::mat4 modelMatrix;

    public:
        Transform() : modelMatrix(1){}
        Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

        glm::vec3 GetPosition();
        glm::vec3 GetRotation();
        glm::vec3 GetScale();

        void SetPosition(glm::vec3 position);
        void SetRotation(glm::vec3 rotation);
        void SetScale(glm::vec3 scale); //unimplemented

        void Translate(glm::vec3 translation);
        void Rotate(glm::vec3 axis, float radians);
        void Scale(glm::vec3 scale);
        glm::mat4 GetModelMatrix() const;

    };


}


