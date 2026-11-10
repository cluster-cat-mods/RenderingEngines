#pragma once
#include <glm/vec3.hpp>
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "Model.h"

namespace core
{
    class Scene
    {
    private:
        std::string name =  "Scene";
        std::list<core::Model> models;
        std::list<Camera> cameras;
        std::list<core::Light> lights;

        core::Camera* activeCamera{};

        glm::vec4 ambientColour = glm::vec4(0.16f, 0.21f, 0.32f, 1.0f);
        float ambientIntensity = 0.1f;

        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    public:

        Scene() = default;

        std::string GetName();
        void SetName(std::string);

        core::Model* Instantiate(core::Model);
        core::Camera* Instantiate(core::Camera);
        core::Light* Instantiate(core::Light);

        std::list<core::Model>* GetModels();
        std::list<core::Camera>* GetCameras();
        std::list<core::Light>* GetLights();

        void SetActiveCamera(core::Camera*);
        core::Camera* GetActiveCamera();

        glm::vec4 GetAmbientColour();
        float GetAmbientIntensity();
        void SetAmbientColour(glm::vec4);
        void SetAmbientIntensity(float);

        glm::vec3 GetWorldUp();
        void SetWorldUp(glm::vec3);

    };
}

