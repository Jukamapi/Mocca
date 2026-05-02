#include "application.h"

#include "Mocca/core/input.h"
#include "Sandbox/experiments/test_feature.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <thread>

// TODO: implement the vkguide newer architecture and break the methods/files better

// TODO: in future implement component-based architecture

Application::Application(uint32_t width, uint32_t height, const std::string& title)
    : m_window(width, height, title), m_context(m_window), m_renderer(m_context, m_window.getDrawableSize())
{
    m_window.onEvent = [this](const Event& event) { m_eventQueue.push_back(event); };
}

void Application::run()
{
    m_isRunning = true;
    auto lastTime = std::chrono::high_resolution_clock::now();

    // onInit();
    m_renderer.pushFeature(std::make_unique<TestFeature>());

    while(m_isRunning)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::min(std::chrono::duration<float>(currentTime - lastTime).count(), 0.1f);
        lastTime = currentTime;

        m_window.pollEvents();

        processEvents();

        processInputs();

        if(m_isMinimized)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            lastTime = std::chrono::high_resolution_clock::now();
            continue;
        }

        // onUpdate(deltaTime);

        m_renderer.drawFrame();
    }

    // onShutdown();
}

void Application::processInputs()
{
    if(Input::isKeyDown(Key::Escape))
    {
        m_isRunning = false;
    }
}

void Application::processEvents()
{
    for(const Event& e : m_eventQueue)
    {

        switch(e.type)
        {
        case EventType::WindowClose:
            m_isRunning = false;
            break;

        case EventType::WindowMinimize:
            m_isMinimized = true;
            break;

        case EventType::WindowRestore:
            m_isMinimized = false;
            break;

        case EventType::WindowResize:
            // handle resize, prob set a flag to get resized
            break;

        default:
            break;
        }
    }

    m_eventQueue.clear();
}

Application::~Application()
{
    vkDeviceWaitIdle(m_context.getDeviceHandle());
}