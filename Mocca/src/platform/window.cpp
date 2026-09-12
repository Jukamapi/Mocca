#include "window.h"

#include "core/event.h"
#include "core/input.h"
#include "core/types.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <imgui_impl_sdl2.h>

#include <cassert>
#include <stdexcept>

// TODO MOCCA: move the windowCount to static?
int Window::s_windowCount = 0;

// TODO MOCCA: try to move the camera movement etc into one place, right now its in window.cpp, camera.cpp,
// application.cpp, sandboxapp.cpp...

Window::Window(uint32_t width, uint32_t height, const std::string title)
    : m_appName(title),
      m_width(width),
      m_height(height)
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

// helper for key presses
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
    case SDL_SCANCODE_LSHIFT:
        return Key::LeftShift;
    case SDL_SCANCODE_LCTRL:
        return Key::LeftCtrl;
    case SDL_SCANCODE_X:
        return Key::X;
    default:
        return Key::Unknown;
    }
}

void Window::pollEvents()
{
    SDL_Event event;

    bool imguiKeyboard = false;
    bool imguiMouse = false;
    if(ImGui::GetCurrentContext() != nullptr)
    {
        ImGuiIO& io = ImGui::GetIO();
        imguiKeyboard = io.WantCaptureKeyboard;
        imguiMouse = io.WantCaptureMouse;
    }

    while(SDL_PollEvent(&event))
    {

        ImGui_ImplSDL2_ProcessEvent(&event);

        switch(event.type)
        {

        // window events
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

        // keyboard inputs
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if(!imguiKeyboard)
            {
                Key key = translateSdlKey(event.key.keysym.scancode);
                Input::setKeyState(key, event.type == SDL_KEYDOWN);
            }
            break;

        // mouse inputs
        case SDL_MOUSEMOTION:
            if(!imguiMouse)
            {
                Input::setMousePosition(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));

                Input::addMouseDelta(static_cast<float>(event.motion.xrel), static_cast<float>(event.motion.yrel));
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if(!imguiMouse)
            {
                bool pressed = (event.type == SDL_MOUSEBUTTONDOWN);

                if(event.button.button == SDL_BUTTON_LEFT)
                    Input::setMouseButtonState(MouseButton::Left, pressed);
                else if(event.button.button == SDL_BUTTON_RIGHT)
                    Input::setMouseButtonState(MouseButton::Right, pressed);
                else if(event.button.button == SDL_BUTTON_MIDDLE)
                    Input::setMouseButtonState(MouseButton::Middle, pressed);
            }
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
