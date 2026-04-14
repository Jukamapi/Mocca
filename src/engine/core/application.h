#pragma once

#include "engine/window/window.h"
#include "engine/renderer/renderer.h"
#include "engine/vk/context.h"

#include <cstdint>
#include <string>
#include <optional>

class Application
{
public:

    Application(uint32_t width, uint32_t height, const std::string& title);

    void run();

private:
    // calls destructors in reverse order, important ordering for now
    Window m_window;
    Context m_context;
    Renderer m_renderer;
};