#include "window.h"
#include "Mocca/core/event.h"
#include "Mocca/core/input.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <cassert>
#include <stdexcept>


int Window::s_windowCount = 0;

Window::Window(uint32_t width, uint32_t height, const std::string title)
    : m_appName(title), m_width(width), m_height(height)
{
    if(s_windowCount == 0)
    {
        if(SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            throw std::runtime_error("failed to init SDL!");
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

    if(!m_window)
    {
        throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
    }

    queryExtensions();

    s_windowCount++;
}

Window::~Window()
{
    if(m_window)
    {
        SDL_DestroyWindow(m_window);
    }

    s_windowCount--;
    if(s_windowCount == 0)
    {
        SDL_Quit();
    }
}

// helper
Key translateSdlKey(SDL_Scancode code)
{
    switch(code)
    {
    case SDL_SCANCODE_ESCAPE:
        return Key::Escape;
    case SDL_SCANCODE_W:
        return Key::W;
    case SDL_SCANCODE_A:
        return Key::A;
    case SDL_SCANCODE_S:
        return Key::S;
    case SDL_SCANCODE_D:
        return Key::D;
    case SDL_SCANCODE_SPACE:
        return Key::Space;
    default:
        return Key::Unknown;
    }
}

void Window::pollEvents()
{
    SDL_Event event;
    Key myKey;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_QUIT:
            if(onEvent)
                onEvent({EventType::WindowClose});
            break;

        case SDL_WINDOWEVENT:
            switch(event.window.event)
            {
            case SDL_WINDOWEVENT_MINIMIZED:
                if(onEvent)
                    onEvent({EventType::WindowMinimize});
                break;

            case SDL_WINDOWEVENT_RESTORED:
                if(onEvent)
                    onEvent({EventType::WindowRestore});
                break;

            case SDL_WINDOWEVENT_SIZE_CHANGED:
            case SDL_WINDOWEVENT_RESIZED:
                m_width = static_cast<uint32_t>(event.window.data1);
                m_height = static_cast<uint32_t>(event.window.data2);
                if(onEvent)
                    onEvent({EventType::WindowResize, m_width, m_height});
                break;
            }
            break;

        case SDL_KEYDOWN:
            myKey = translateSdlKey(event.key.keysym.scancode);
            Input::setKeyState(myKey, true);
            break;
        case SDL_KEYUP:
            myKey = translateSdlKey(event.key.keysym.scancode);
            Input::setKeyState(myKey, false);
            break;

        case SDL_MOUSEMOTION:
            Input::mouseX = event.motion.x;
            Input::mouseY = event.motion.y;
            break;
        }
    }
}

void Window::queryExtensions()
{
    SDL_Vulkan_GetInstanceExtensions(m_window, &m_sdlExtensionCount, nullptr);
    m_sdlExtensions.resize(m_sdlExtensionCount);
    SDL_Vulkan_GetInstanceExtensions(m_window, &m_sdlExtensionCount, m_sdlExtensions.data());
}

Extent Window::getDrawableSize() const
{
    int w, h;
    SDL_Vulkan_GetDrawableSize(m_window, &w, &h);

    return {static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
}
