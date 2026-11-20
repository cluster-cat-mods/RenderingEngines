#pragma once
#include "Transform.h"
#include <string>

namespace core
{
    class Object
    {
    protected:
        std::string name = "Object";
        bool enabled = true;
    public:
        core::Transform transform = core::Transform();

        Object(){}

        std::string GetName();
        void SetName(std::string);

        void SetEnabled(bool);
        bool IsEnabled();

    };


}
