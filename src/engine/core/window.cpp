#include "window.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <stdexcept>

Window::Window(int width, int height, const std::string& title)
    : m_width(width), m_height(height)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::runtime_error("Failed to init SDL");
    }

    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
    );

    if (!m_window)
    {
        throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
    }
}

Window::~Window()
{
    if (m_window)
    {
        SDL_DestroyWindow(m_window);
    }
    SDL_Quit();
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
            }
            break;
        }

    }
}

