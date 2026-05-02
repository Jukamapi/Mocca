#pragma once

#include "Mocca/vulkan/commands/deletion_queue.h"

#include <volk.h>

#include <array>
#include <memory>

struct QueueFamilyIndices;
class CommandPool;

class FrameManager
{
public:
    FrameManager(const QueueFamilyIndices& indices, VkDevice device);
    ~FrameManager();
    FrameManager(const FrameManager&) = delete;
    FrameManager& operator=(const FrameManager&) = delete;
    FrameManager(FrameManager&&) = delete;
    FrameManager& operator=(FrameManager&&) = delete;

    // note:  if i want to multithread later i need to snapshot global data
    struct FrameData
    {
        std::unique_ptr<CommandPool> commandPool;

        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence renderFence;

        // use deletion queue for all the data in here as the  lifetime is a bit complicated
        DeletionQueue deletionQueue;
    };

    FrameData& getCurrentFrame()
    {
        return m_frames[m_currentFrameIndex];
    }
    DeletionQueue& getCurrentDeletionQueue()
    {
        return getCurrentFrame().deletionQueue;
    }
    const FrameData& getCurrentFrame() const
    {
        return m_frames[m_currentFrameIndex];
    }
    const DeletionQueue& getCurrentDeletionQueue() const
    {
        return getCurrentFrame().deletionQueue;
    }

    void advance();

private:
    std::array<FrameData, 2> m_frames;
    uint32_t m_currentFrameIndex{0};

    VkDevice m_device;
};