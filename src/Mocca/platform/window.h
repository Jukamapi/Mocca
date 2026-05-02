#pragma once

#include "Mocca/core/event.h"
#include "Mocca/core/types.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

struct SDL_Window;

class Window
{
public:
    using EventCallbackFn = std::function<void(Event&)>;

    Window(uint32_t width, uint32_t height, const std::string title);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    void pollEvents();

    Extent getDrawableSize() const;

    uint32_t getHeight() const
    {
        return m_height;
    }
    uint32_t getWidth() const
    {
        return m_width;
    }

    SDL_Window* getNativeWindow() const
    {
        return m_window;
    }
    const std::string& getAppName() const
    {
        return m_appName;
    }

    uint32_t getExtensionCount() const
    {
        return m_sdlExtensionCount;
    }
    std::vector<const char*> getExtensions() const
    {
        return m_sdlExtensions;
    }

    std::function<void(const Event&)> onEvent;

private:
    void queryExtensions();

    const std::string m_appName;
    SDL_Window* m_window{nullptr};

    uint32_t m_width{0};
    uint32_t m_height{0};

    uint32_t m_sdlExtensionCount{0};
    std::vector<const char*> m_sdlExtensions{};

    static int s_windowCount;
};