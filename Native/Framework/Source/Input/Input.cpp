#include "Input.h"
#include <glfw3.h>
#include <glfw3native.h>

namespace Odyssey::Framework
{
    std::array<std::bitset<1024>, Input::KeyState::Size> Input::keyInput;
    double Input::mouseX = 0.0;
    double Input::mouseY = 0.0;

    void Input::Initialize()
    {
        glfwInit();

        keyInput[KeyState::Up].reset();
        keyInput[KeyState::Press].reset();
    }

    bool Input::GetKeyPress(KeyCode keyCode)
    {
        return keyInput[KeyState::Press][keyCode];
    }

    bool Input::GetKeyDown(KeyCode keyCode)
    {
        return keyInput[KeyState::Down][keyCode];
    }

    bool Input::GetKeyUp(KeyCode keyCode)
    {
        return keyInput[KeyState::Up][keyCode];
    }

    void Input::RegisterKeyPress(int key, int scanCode)
    {
        // Flag as key press and down
        keyInput[KeyState::Press][key] = true;
        keyInput[KeyState::Down][key] = true;

        // Clear key up
        keyInput[KeyState::Up][key] = false;
    }

    void Input::RegisterKeyDown(int key, int scanCode)
    {
        // Flag as key down
        keyInput[KeyState::Down][key] = true;

        // Clear key press and up
        keyInput[KeyState::Press][key] = false;
        keyInput[KeyState::Up][key] = false;
    }

    void Input::RegisterKeyUp(int key, int scanCode)
    {
        // Flag as key up
        keyInput[KeyState::Up][key] = true;

        // Clear key press and down
        keyInput[KeyState::Press][key] = false;
        keyInput[KeyState::Down][key] = false;
    }

    void Input::RegisterMouseMove(double x, double y)
    {
        mouseX = x;
        mouseY = y;
    }

    void Input::RegisterMousePosition(double x, double y, bool entered)
    {
        if (entered)
        {
            mouseX = x;
            mouseY = y;
        }
        else
        {
            mouseX = mouseY = 0.0;
        }
    }
}