#pragma once

#include <array>
#include <glm/vec2.hpp>

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
    static void newFrame();

    static bool isKeyDown(Key key);
    static bool isKeyPressed(Key key);  // pressed this frame
    static bool isKeyReleased(Key key); // released this frame

    static bool isMouseButtonDown(MouseButton button);
    static bool isMouseButtonPressed(MouseButton button);

    static glm::vec2 getMousePosition();
    static glm::vec2 getMouseDelta();


    static void setKeyState(Key key, bool pressed);
    static void setMouseButtonState(MouseButton button, bool pressed);
    static void setMousePosition(float x, float y);
    static void addMouseDelta(float dx, float dy);

private:
    static inline std::array<bool, (size_t)Key::COUNT> s_currKeys{};
    static inline std::array<bool, (size_t)Key::COUNT> s_prevKeys{};

    static inline std::array<bool, (size_t)MouseButton::COUNT> s_currMouseButtons{};
    static inline std::array<bool, (size_t)MouseButton::COUNT> s_prevMouseButtons{};

    static inline glm::vec2 s_mousePos{0.0f, 0.0f};
    static inline glm::vec2 s_mouseDelta{0.0f, 0.0f};
};
