#include "application.h"

#include <chrono>
#include <thread>

Application::Application(int width, int height, const std::string& title)
    : m_window(width, height, title),
    m_context(m_window),
    m_renderer(m_context)
{}

void Application::run()
{
    while (!m_window.shouldClose())
    {
        m_window.pollEvents();

        if (m_window.isMinimized())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        m_renderer.draw();
    }
}