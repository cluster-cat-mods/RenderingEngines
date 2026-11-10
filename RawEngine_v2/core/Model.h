#pragma once

#include <vector>
#include <glm/ext/matrix_float4x4.hpp>
#include "mesh.h"
#include "Object.h"

namespace core
{
    class Model : public Object
    {
    private:
        std::vector<core::Mesh> meshes;
    public:
        Model(std::vector<core::Mesh> meshes) : meshes(meshes){}
        Model(std::vector<core::Mesh> meshes, Transform transform, std::string name);
        Model(std::vector<core::Mesh> meshes, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::string name);

        void render(GLenum drawMode);
    };


}
