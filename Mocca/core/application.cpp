#include "application.h"

#include "Mocca/core/input.h"
#include "Mocca/core/timer.h"


#include <chrono>
#include <cstdint>
#include <thread>

// TODO: implement the vkguide newer architecture and break the methods/files better

// TODO: in future implement component-based architecture

// TODO: heap allocate when: need to replace, polymorphism or tied to GPU lifetime

Application::Application(uint32_t width, uint32_t height, const std::string& title)
    : m_window(width, height, title), m_context(m_window),
      m_renderer(m_context, [this]() -> Extent { return m_window.getDrawableSize(); })
{
    m_window.onEvent = [this](const Event& event) { m_eventQueue.push_back(event); };
}

void Application::run()
{
    m_isRunning = true;
    Timer timer;

    onInit();

    while(m_isRunning)
    {
        m_window.pollEvents();
        processEvents();

        if(m_isMinimized)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            timer.reset();
            continue;
        }

        float dt = timer.getDeltaTime();

        tickLogic(dt);
        tickRender(dt);
    }

    onShutdown();
}

// TODO: maybe get the event handling into seperate file?

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
            m_renderer.markSwapchainDirty();
            break;

        case EventType::WindowRestore:
            m_isMinimized = false;
            m_renderer.markSwapchainDirty();
            break;

        case EventType::WindowResize:
            m_renderer.markSwapchainDirty();
            break;


        default:
            break;
        }
    }

    m_eventQueue.clear();
}

void Application::tickLogic(float dt)
{
    // global input
    processInputs();

    // sandbox logic
    onUpdate(dt);

    // feature logic
    for(auto& feature : m_renderer.getFeatures())
    {
        feature->onUpdate(dt);
    }
}

void Application::tickRender(float dt)
{
    // internally calls each features' onRender
    m_renderer.drawFrame();

    onImgui();
}

Application::~Application()
{
    vkDeviceWaitIdle(m_context.getDeviceHandle());
}
