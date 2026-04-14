#pragma once

#include "engine/vk/utils.h"
#include "engine/core/window.h"
#include "engine/renderer/renderer.h"
#include "engine/vk/context.h"

class Application
{
public:

    Application(int width, int height, const std::string& title);

    void run();

private:
    // calls destructors in reverse order
    Window m_window;
    Context m_context;
    Renderer m_renderer;
};