#pragma once

#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace core
{
    class Camera
    {
    private:
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 10.0f);
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 dir = glm::normalize(pos - target);
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(worldUp, dir));
        glm::vec3 up = glm::cross(dir, right);

        float fov = 45.0f;
        float near = 0.1f;
        float far = 100.0f;

    public:
        Camera(){}
        Camera(glm::vec3 pos, glm::vec3 target, float fov, float near, float far);

        glm::mat4 VM() const;
        glm::mat4 PM(int winWidth, int winHeight) const;

        void Translate(glm::vec3 translation);
        void Rotate(glm::vec3 rotation);
        void ChangeFOV(float fov);

    };
}
