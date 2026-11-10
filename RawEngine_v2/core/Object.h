#pragma once
#include "Transform.h"
#include <string>

namespace core
{
    class Object
    {
    protected:
        std::string name = "Object";
    public:
        core::Transform transform = core::Transform();

        Object(){}

        std::string GetName();
        void SetName(std::string);
    };


}
