#pragma once
#include "Object.h"

namespace core
{
    class Light : public Object
    {
    private:
        glm::vec4 colour;
        //col, spec int,
        //copy assignments?

    public:
        Light(Transform transform, glm::vec4 colour);
        ~Light() = default;

        glm::vec4 GetColour();
        void SetColour(glm::vec4 colour);
    };
}
