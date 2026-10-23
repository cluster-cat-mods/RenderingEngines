#include "Input.h"

namespace core
{
    core::Input::KeyMap::KeyMap(int key, int action, bool held, std::function<void()> func) //copying bad
    {
        this->key = key;
        this->action = action;
        this->holdKey = held;
        this->func = func;
        this->isHeld = false;
    }

    void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        std::cout << "received callback" << std::endl;
        auto* input = static_cast<Input*>
        (
            glfwGetWindowUserPointer(window)
        );

        if (!input)
        {
            std::cout << "No input found" << std::endl;
            return;
        }

        for (auto& keyMap : input->keyMaps)
        {
            if (key == keyMap.key && action == keyMap.action)
            {
                if (keyMap.holdKey && action == GLFW_PRESS) keyMap.isHeld = true;
                keyMap.func();
            }
            else if (keyMap.isHeld && action == GLFW_RELEASE) keyMap.isHeld = false;

        }
    }

    int core::Input::AddKeyMap(KeyMap keyMap)
    {
        this->keyMaps.push_back(keyMap);
        return this->keyMaps.size();
    }

    int core::Input::AddKeyMap(int key, int action, bool held, std::function<void()> func)
    {
        keyMaps.push_back(KeyMap(key, action, held, func));
        return this->keyMaps.size();
    }

    void core::Input::RemoveKeyMap(int index)
    {
        this->keyMaps.erase(this->keyMaps.begin() + index);
    }

    void core::Input::StartInputCallback(GLFWwindow* window)
    {
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, Input::KeyCallback);
        std::cout << "Input Callback started" << std::endl;
    }

    void core::Input::ProccesHeldKeys()
    {
        for (auto& keyMap : this->keyMaps) if (keyMap.isHeld) keyMap.func();
    }


}

