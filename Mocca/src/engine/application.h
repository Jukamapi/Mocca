#pragma once

#include "Mocca/core/event.h"
#include "Mocca/platform/window.h"
#include "Mocca/renderer/renderer.h"
#include "Mocca/vulkan/core/context.h"


#include <cstdint>
#include <string>
#include <vector>

class Application
{
public:
    Application(uint32_t width, uint32_t height, const std::string& title);
    virtual ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void run();

protected:
    virtual void onInit() {}
    virtual void onUpdate(float deltaTime) {}
    virtual void onImgui() {}
    virtual void onShutdown() {}

    // call destructors in reverse order, important ordering for now
    Window m_window;
    Context m_context;
    Renderer m_renderer;

private:
    void processEvents();
    void processInputs();
    void tickLogic(float deltaTime);
    void tickRender(float deltaTime);


    std::vector<Event> m_eventQueue;
    bool m_isRunning{false};
    bool m_isMinimized{false};
};