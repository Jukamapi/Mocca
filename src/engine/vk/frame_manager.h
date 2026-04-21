#pragma once

#include "engine/vk/deletion_queue.h"
#include <vulkan/vulkan.h>

#include <array>

class FrameManager
{
public:
    FrameManager();
    ~FrameManager();
    FrameManager(const FrameManager&) = delete;
    FrameManager& operator=(const FrameManager&) = delete;
    FrameManager(FrameManager&&) = delete;
    FrameManager& operator=(FrameManager&&) = delete;
    //  if i want to multithread later i need to snapshot global data
    struct FrameData
    {
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
        //  use deletion queue for all the data in here as the  lifetime is a bit complicated
        DeletionQueue deletionQueue;
    };

    FrameData& getCurrentFrame() { return m_frames[m_currentFrameIndex]; }
    DeletionQueue& getCurrentDeletionQueue() { return getCurrentFrame().deletionQueue; }
    const FrameData& getCurrentFrame() const { return m_frames[m_currentFrameIndex]; }
    const DeletionQueue& getCurrentDeletionQueue() const { return getCurrentFrame().deletionQueue; }

    void advance();

private:
    std::array<FrameData, 2> m_frames;
    uint32_t m_currentFrameIndex{0};

    VkDevice m_device;
};