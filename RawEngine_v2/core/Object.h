#pragma once
#include "model.h"
#include "Transform.h"

namespace core
{
    class Object
    {
    public:
        core::Model model;
        core::Transform transform;

        Object();
    };
}
