#pragma once

#include "engine/window/window.h"
#include "engine/core/event_handler.h"
#include "engine/renderer/renderer.h"
#include "engine/vk/context.h"

#include <cstdint>
#include <string>

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
    Window m_window;
    Context m_context;
    Renderer m_renderer;
    EventHandler m_eventHandler;


    bool m_isRunning{true};
};