#pragma once
#include "Object.h"

namespace core
{
    class Light : public Object
    {
    private:
        glm::vec4 color;
        //col, spec int,
        //copy assignments?

    public:
        Light(Transform transform, glm::vec4 color);
        ~Light() = default;

        glm::vec4 GetColor();
        void SetColor(glm::vec4 color);
    };
}
