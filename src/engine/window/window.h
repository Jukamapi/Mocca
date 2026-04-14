#pragma once

#include <string>
#include <cstdint>

struct SDL_Window;

class Window {
public:
    Window(uint32_t width, uint32_t height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    //  Make Event class that every event inherits from,
    //  single callback in window
    //  Use EventDispatcher in Application

    void pollEvents();

    bool shouldClose() const { return m_shouldClose; }
    bool isMinimized() const { return m_isMinimized; }

    uint32_t getHeight() const { return m_height; }
    uint32_t getWidth() const { return m_width; }

    // for surface created in context
    SDL_Window* getNativeWindow() const { return m_window; }

private:
    SDL_Window* m_window{ nullptr };
    bool m_shouldClose{ false };
    bool m_isMinimized{ false };

    uint32_t m_width{ 0 };
    uint32_t m_height{ 0 };
};