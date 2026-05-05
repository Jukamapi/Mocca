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
    COUNT
};

class Input
{
public:
    inline static int mouseX = 0;
    inline static int mouseY = 0;
    inline static std::array<bool, (size_t)Key::COUNT> keys = {false};
    static void setKeyState(Key key, bool pressed)
    {
        keys[(size_t)key] = pressed;
    }
    static bool isKeyDown(Key key)
    {
        return keys[(size_t)key];
    }
};
