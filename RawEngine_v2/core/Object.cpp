#include "Object.h"

namespace core
{
    std::string Object::GetName()
    {
        return name;
    }
    void Object::SetName(std::string name)
    {
        this->name = name;
    }
}
