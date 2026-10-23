#include "Camera.h"
#include <glm/ext/matrix_clip_space.hpp>

namespace core
{
    Camera::Camera(Transform transform, float targetDistance, float fov, float near, float far)
    {
        this->name = "Camera";
        this->transform = transform;
        this->targetDistance = targetDistance;
        this->fov = fov;
        this->near = near;
        this->far = far;
    }
    Camera::Camera(glm::vec3 position, glm::vec3 rotation, float targetDistance, float fov, float near, float far)
    {
        this->name = "Camera";
        this->transform = Transform(position, rotation, glm::vec3(1.0f));
        this->targetDistance = targetDistance;
        this->fov = fov;
        this->near = near;
        this->far = far;
    }

    glm::vec3 Camera::GetForward() const
    {
        return glm::normalize(transform.GetModelMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    }
    glm::vec3 Camera::GetTarget() const
    {
        return static_cast<Transform>(transform).GetPosition() + GetForward() * targetDistance;
    }

    glm::mat4 Camera::VM() const { return glm::lookAt(static_cast<Transform>(transform).GetPosition(), GetTarget(), worldUp);}
    glm::mat4 Camera::PM(int winWidth, int winHeight) const { return glm::perspective(glm::radians(fov), static_cast<float>(winWidth) / static_cast<float>(winHeight), near, far);}

    void Camera::ChangeFOV(float fovOffset)
    {
        this->fov += fovOffset;
    }

}
