#pragma once
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <functional>

namespace core
{
    class Input
    {
    protected:
        struct KeyMap
        {
            int key;
            int action;
            bool holdKey;
            std::function<void()> func;
            bool isHeld;

            KeyMap(int key, int action, bool holdKey, std::function<void()> func);
        };

        std::vector<KeyMap> keyMaps;

    public:
        int AddKeyMap(KeyMap keyMap);
        int AddKeyMap(int key, int action, bool holdKey, std::function<void()> func);
        void RemoveKeyMap(int index);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void StartInputCallback(GLFWwindow* window);
        void ProccesHeldKeys();


    };


}


