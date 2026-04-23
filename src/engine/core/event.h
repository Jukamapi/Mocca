#pragma once
#include <cstdint>

enum class EventType
{
    None = 0,
    WindowClose,
    WindowResize,
    WindowMinimize,
    WindowRestore
};

struct Event
{
    EventType type;
    uint32_t width;
    uint32_t height;
};