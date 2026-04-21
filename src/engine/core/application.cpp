#include "application.h"
#include "engine/core/event_system.h"

#include <chrono>
#include <thread>
#include <algorithm>

Application::Application(uint32_t width, uint32_t height, const std::string& title)
    : m_window(width, height, title), m_context(m_window), m_renderer(), m_eventHandler()
{
    m_window.onEvent = [this](const Event& event)
    {
        m_eventHandler.handle(event);
    };
}

void Application::run()
{
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!m_window.shouldClose())
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::min(std::chrono::duration<float>(currentTime - lastTime).count(), 0.1f);
        lastTime = currentTime;

        m_window.pollEvents();

        while (EventSystem::instance().hasEvents())
        {
            Event event = EventSystem::instance().pop();
            m_eventHandler.handle(event);
        }

        if (m_window.isMinimized())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            lastTime = std::chrono::high_resolution_clock::now();
            continue;
        }

        m_renderer.drawFrame();
    }
}

Application::~Application()
{
    //vkDeviceWaitIdle(m_context.getDevice());
}