#pragma once
#include "Transform.h"
#include <string>

namespace core
{
    class Object
    {

    public:
        std::string name = "Object";
        core::Transform transform = core::Transform();

        Object(){}
    };


}
