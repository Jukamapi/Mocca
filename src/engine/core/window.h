#pragma once

#include <string>

struct SDL_Window;

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    // when i start polling for keys i should add a bunch of bools that change
    // depending on the keypress, this way
    // i can keep window seperate

    void pollEvents();

    bool shouldClose() const { return m_shouldClose; }
    bool isMinimized() const { return m_isMinimized; }

    int getHeight() const { return m_height; }
    int getWidth() const { return m_width; }

    // we create surface here and get extensions!!!


private:
    SDL_Window* m_window{ nullptr };
    bool m_shouldClose{ false };
    bool m_isMinimized{ false };

    int m_width{ 0 };
    int m_height{ 0 };
};