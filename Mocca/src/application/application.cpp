#include "application.h"

#include "core/input.h"
#include "core/timer.h"

#include "renderer/renderer.h"
#include "resource/asset_manager.h"
#include "scene/scene.h"

#include <chrono>
#include <cstdint>
#include <thread>

// TODO MOCCA: optimize the rendering, check all the waits if they are needed/in good place

// TODO MOCCA: implement a class that will handle the events/inputs

// this utilizes extent provider to somewhat respect the boundaries of architecture
// and so it doesn't have to include sdl as much since it's big
Application::Application(uint32_t width, uint32_t height, const std::string& title)
    : m_window(width, height, title)
{
    // event callback
    m_window.onEvent = [this](const Event& event) { m_eventQueue.push_back(event); };

    m_renderer = std::make_unique<Renderer>(m_window, [this]() -> Extent { return m_window.getDrawableSize(); });

    const auto& context = m_renderer->getContext();
    VkDevice device = context.getLogicalDevice().getHandle();
    VkQueue graphicsQueue = context.getLogicalDevice().getGraphicsQueue();
    const auto& indices = context.getPhysicalDevice().getQueueFamilyIndices();
    VmaAllocator allocator = context.getVmaAlloc().getVmaAllocator();

    m_assetManager = std::make_unique<AssetManager>(
        device,
        graphicsQueue,
        indices,
        allocator,
        m_renderer->getMaterialLayout().getHandle()
    );

    m_scene = std::make_unique<Scene>();
}

void Application::run()
{
    m_isRunning = true;
    Timer timer;

    onInit();

    while(m_isRunning)
    {
        Input::newFrame();

        m_window.pollEvents();
        processEvents();

        // lower utilized resources when minimized
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
            m_renderer->markSwapchainDirty();
            break;

        case EventType::WindowRestore:
            m_isMinimized = false;
            m_renderer->markSwapchainDirty();
            break;

        case EventType::WindowResize:
            m_renderer->markSwapchainDirty();
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
    onInput();

    // sandbox logic
    onTick(dt);

    if(m_scene)
    {
        m_scene->update();
    }

    // feature logic
    for(auto& feature : m_renderer->getFeatures())
    {
        if(feature->isEnabled())
            feature->onUpdate(dt);
    }
}

void Application::tickRender(float dt)
{
    if(!m_renderer->beginFrame())
        return;

    float aspect = (float)m_renderer->getExtent().width / (float)m_renderer->getExtent().height;
    GlobalRenderData frameData = m_scene->getRenderData(aspect);
    m_renderer->updateGlobalUniforms(frameData);

    m_renderer->beginUiFrame();

    onImgui();

    for(auto& feature : m_renderer->getFeatures())
    {
        if(feature->isEnabled())
            feature->onImgui();
    }

    m_renderer->endUiFrame();

    // calls each features' onRender
    m_renderer->endFrame();
}

void Application::onShutdown()
{
    vkDeviceWaitIdle(m_renderer->getContext().getLogicalDevice().getHandle());
}

void Application::close()
{
    m_isRunning = false;
}


Application::~Application() {}
