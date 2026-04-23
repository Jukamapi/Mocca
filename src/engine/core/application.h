#pragma once

#include "engine/core/event.h"
#include "engine/window/window.h"
#include "engine/renderer/renderer.h"
#include "engine/vk/context.h"

#include <cstdint>
#include <string>
#include <vector>

class Application
{
public:

    Application(uint32_t width, uint32_t height, const std::string& title);
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void run();

private:
    // call destructors in reverse order, important ordering for now
    void processEvents();
    void processInputs();

    Window m_window;
    Context m_context;
    Renderer m_renderer;

    std::vector<Event> m_eventQueue;
    bool m_isRunning{ false };
    bool m_isMinimized{ false };
};