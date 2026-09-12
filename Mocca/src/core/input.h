#pragma once

#include <array>

enum class Key
{
    Unknown = 0,
    Escape,
    W,
    A,
    S,
    D,
    Space,
    LeftShift,
    LeftCtrl,
    X,
    COUNT
};


enum class MouseButton
{
    Left = 0,
    Right,
    Middle,
    COUNT
};

class Input
{
public:
    inline static int mouseX = 0;
    inline static int mouseY = 0;
    inline static int mouseDeltaX = 0;
    inline static int mouseDeltaY = 0;

    inline static std::array<bool, (size_t)Key::COUNT> keys = {false};
    inline static std::array<bool, (size_t)MouseButton::COUNT> mouseButtons = {false};

    static void setKeyState(Key key, bool pressed)
    {
        keys[(size_t)key] = pressed;
    }
    static bool isKeyDown(Key key)
    {
        return keys[(size_t)key];
    }

    static void setMouseButtonState(MouseButton button, bool pressed)
    {
        mouseButtons[(size_t)button] = pressed;
    }
    static bool isMouseButtonDown(MouseButton button)
    {
        return mouseButtons[(size_t)button];
    }

    static void resetDeltas()
    {
        mouseDeltaX = 0;
        mouseDeltaY = 0;
    }
};
