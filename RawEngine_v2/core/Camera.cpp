#include "Camera.h"

namespace core
{
    Camera::Camera(glm::vec3 pos, glm::vec3 target, float fov, float near, float far)
    {
        this->pos = pos;
        this->target = target;
        this->fov = fov;
        this->near = near;
        this->far = far;
    }

    glm::mat4 Camera::VM() const { return glm::lookAt(pos, target, up);}
    glm::mat4 Camera::PM(int winWidth, int winHeight) const { return glm::perspective(glm::radians(fov), static_cast<float>(winWidth) / static_cast<float>(winHeight), near, far);}

    void Camera::Translate(glm::vec3 translation)
    {
        this->pos += translation;
        this->target += translation;
    }
    void Camera::Rotate(glm::vec3 rotation)
    {
        
    }
    void Camera::ChangeFOV(float fovOffset)
    {
        this->fov += fovOffset;
    }

}
