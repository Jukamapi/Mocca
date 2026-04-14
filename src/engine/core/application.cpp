#include "application.h"

#include <chrono>
#include <thread>

Application::Application(uint32_t width, uint32_t height, const std::string& title)
    : m_window(width, height, title),
    m_context(m_window),
    m_renderer(m_context)
{

}

void Application::run()
{
    while (!m_window.shouldClose())
    {
        m_window.pollEvents();

        //  TODO: add delta time/timestep

        if (m_window.isMinimized())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        m_renderer.drawFrame();
    }
}