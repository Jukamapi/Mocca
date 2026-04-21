#pragma once

#include "engine/core/event.h"

class Context;

class EventHandler
{
public:
    EventHandler();
    void handle(const Event& event);

private:

    void onWindowResize(uint32_t width, uint32_t height);
    void onMouseMove(int32_t x, int32_t y);

};