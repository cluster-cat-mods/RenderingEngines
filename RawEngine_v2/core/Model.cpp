#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Material.h"

namespace core {
    void Model::render(GLenum drawMode)
    {
        for (int i = 0; i < meshes.size(); ++i)
        {
            meshes[i].render(drawMode);
        }
    }

    Model::Model(std::vector<Mesh> meshes, Transform transform, std::string name = "Model")
    {
        this->name = name;
        this->meshes = meshes;
        this->transform = transform;
        this->material = Material();
    }
    Model::Model(std::vector<Mesh> meshes, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::string name = "Model")
    {
        this->name = name;
        this->meshes = meshes;
        this->transform = Transform(position, rotation, scale);
    }


}
