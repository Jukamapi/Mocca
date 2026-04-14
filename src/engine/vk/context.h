#pragma once

#include "engine/vk/deletion_queue.h"
#include <vulkan/vulkan.h> // needed here

class Window;


//  if i want to multithread later i need to snapshot global data
struct FrameData
{
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    DeletionQueue deletionQueue;
};

class Context
{
public:
    // .getNativeWindow() to create the surface
    Context(Window& window);
    ~Context();

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;

    uint32_t getWindowWidth() const;
    uint32_t getWindowHeight() const;

    //  todo: getters for vulkan specific

    FrameData& getCurrentFrame() { return m_frames[m_currentFrameIndex]; }
    DeletionQueue& getCurrentDeletionQueue() { return getCurrentFrame().deletionQueue; }


private:
    Window* m_window{ nullptr };

    // todo: instance, device

    // 2 for double buffering
    FrameData m_frames[2];
    uint32_t m_currentFrameIndex{ 0 };
};