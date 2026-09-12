#include "input.h"

void Input::newFrame()
{
    s_prevKeys = s_currKeys;
    s_prevMouseButtons = s_currMouseButtons;
    s_mouseDelta = {0.0f, 0.0f};
}

bool Input::isKeyDown(Key key)
{
    return s_currKeys[static_cast<size_t>(key)];
}

bool Input::isKeyPressed(Key key)
{
    return s_currKeys[static_cast<size_t>(key)] && !s_prevKeys[static_cast<size_t>(key)];
}

bool Input::isKeyReleased(Key key)
{
    return !s_currKeys[static_cast<size_t>(key)] && s_prevKeys[static_cast<size_t>(key)];
}

bool Input::isMouseButtonDown(MouseButton button)
{
    return s_currMouseButtons[static_cast<size_t>(button)];
}

bool Input::isMouseButtonPressed(MouseButton button)
{
    return s_currMouseButtons[static_cast<size_t>(button)] && !s_prevMouseButtons[static_cast<size_t>(button)];
}

glm::vec2 Input::getMousePosition()
{
    return s_mousePos;
}
glm::vec2 Input::getMouseDelta()
{
    return s_mouseDelta;
}

void Input::setKeyState(Key key, bool pressed)
{
    s_currKeys[static_cast<size_t>(key)] = pressed;
}

void Input::setMouseButtonState(MouseButton button, bool pressed)
{
    s_currMouseButtons[static_cast<size_t>(button)] = pressed;
}

void Input::setMousePosition(float x, float y)
{
    s_mousePos = {x, y};
}

void Input::addMouseDelta(float dx, float dy)
{
    s_mouseDelta += glm::vec2{dx, dy};
}