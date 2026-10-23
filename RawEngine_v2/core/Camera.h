#pragma once

#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Object.h"

namespace core
{
    class Camera : public core::Object
    {
    private:
        float targetDistance = 10.0f;
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        float fov = 45.0f;
        float near = 0.1f;
        float far = 100.0f;

    public:
        Camera(){}
        Camera(Transform transform, float targetDistance, float fov, float near, float far);
        Camera(glm::vec3 position, glm::vec3 rotation, float targetDistance, float fov, float near, float far);

        glm::vec3 GetForward() const;
        glm::vec3 GetTarget() const;

        glm::mat4 VM() const;
        glm::mat4 PM(int winWidth, int winHeight) const;

        void ChangeFOV(float fov);

    };
}
