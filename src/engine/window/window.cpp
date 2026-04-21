#include "window.h"
#include "engine/core/event_system.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <stdexcept>
#include <cassert>

static int s_windowCount = 0;

Window::Window(uint32_t width, uint32_t height, const std::string& title)
    : m_appName(title), m_width(width), m_height(height)
{
    if(s_windowCount == 0)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            throw std::runtime_error("Failed to init SDL");
        }
    }


    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        static_cast<int>(width),
        static_cast<int>(height),
        SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
    );

    if (!m_window)
    {
        throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
    }

    s_windowCount++;
}

Window::~Window()
{
    if (m_window)
    {
        SDL_DestroyWindow(m_window);
    }

    s_windowCount--;
    if(s_windowCount == 0)
    {
        SDL_Quit();
    }
}

void Window::pollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            m_shouldClose = true;
            EventSystem::instance().push(WindowCloseEvent{});
            break;

        case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_MINIMIZED:
                m_isMinimized = true;
                break;

            case SDL_WINDOWEVENT_RESTORED:
                m_isMinimized = false;
                break;

            case SDL_WINDOWEVENT_SIZE_CHANGED:
            case SDL_WINDOWEVENT_RESIZED:
                m_width = static_cast<uint32_t>(event.window.data1);
                m_height = static_cast<uint32_t>(event.window.data2);
                EventSystem::instance().push(WindowResizeEvent{m_width, m_height});
                break;
            }
            break;

        case SDL_MOUSEMOTION:
            EventSystem::instance().push(MouseMoveEvent{event.motion.x, event.motion.y});
            break;
        }

    }
}

void Window::queryExtensions()
{
    SDL_Vulkan_GetInstanceExtensions(m_window, &m_sdlExtensionCount, m_sdlExtensions.data());
}