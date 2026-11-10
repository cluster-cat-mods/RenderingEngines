#include "Scene.h"

#include <iostream>

namespace core
{
    std::string Scene::GetName()
    {
        return name;
    }
    void Scene::SetName(std::string name)
    {
        this->name = name;
    }

    core::Model* Scene::Instantiate(core::Model model)
    {
        this->models.push_back(model);
        return &models.back();
    }
    core::Camera* Scene::Instantiate(core::Camera camera)
    {
        this->cameras.push_back(camera);
        return &cameras.back();
    }
    core::Light* Scene::Instantiate(core::Light light)
    {
        this->lights.push_back(light);
        return &lights.back();
    }

    void Scene::SetActiveCamera(core::Camera* camera)
    {
        this->activeCamera = camera;
    }
    core::Camera* Scene::GetActiveCamera()
    {
        if (cameras.empty()) throw std::exception("No instantiated cameras");
        if (activeCamera == nullptr) throw std::exception("No active camera"); // error
        return activeCamera;
    }

    std::list<core::Model>* Scene::GetModels()
    {
        return &models;
    }
    std::list<core::Camera>* Scene::GetCameras()
    {
        return &cameras;
    }
    std::list<core::Light>* Scene::GetLights()
    {
        return &lights;
    }

    glm::vec4 Scene::GetAmbientColour()
    {
        return ambientColour;
    }
    float Scene::GetAmbientIntensity()
    {
        return ambientIntensity;
    }
    void Scene::SetAmbientColour(glm::vec4 ambientColour)
    {
        this->ambientColour = ambientColour;
    }
    void Scene::SetAmbientIntensity(float ambientIntensity)
    {
        this->ambientIntensity = ambientIntensity;
    }

    glm::vec3 Scene::GetWorldUp()
    {
        return worldUp;
    }
    void Scene::SetWorldUp(glm::vec3 worldUp)
    {
        this->worldUp = worldUp;
    }

}
