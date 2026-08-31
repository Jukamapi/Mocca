#pragma once

#include "core/vulkan/vk_types.h"
#include "platform/vulkan/command_pool.h"


#include "resource/deletion_queue.h"
#include "resource/vulkan/allocated_image.h"

#include <volk.h>

#include <array>

struct QueueFamilyIndices;
class CommandPool;

class FrameManager
{
public:
    inline static constexpr uint32_t FRAME_COUNT{2};

    FrameManager(const QueueFamilyIndices& indices, VkDevice device);
    ~FrameManager();
    FrameManager(const FrameManager&) = delete;
    FrameManager& operator=(const FrameManager&) = delete;
    FrameManager(FrameManager&&) = delete;
    FrameManager& operator=(FrameManager&&) = delete;

    // note:  if i want to multithread later i need to snapshot global data
    struct FrameData
    {
        CommandPool commandPool;

        VkSemaphore imageAvailableSemaphore{VK_NULL_HANDLE};
        VkSemaphore renderFinishedSemaphore{VK_NULL_HANDLE};
        VkFence renderFence{VK_NULL_HANDLE};

        DeletionQueue deletionQueue;

        AllocatedImage colorImage;
        AllocatedImage depthImage;

        FrameData(const QueueFamilyIndices& indices, VkDevice device)
            : commandPool(indices, device)
        {
        }
    };


    FrameData& getCurrentFrame()
    {
        return m_frames[m_currentFrameIndex];
    }
    const FrameData& getCurrentFrame() const
    {
        return m_frames[m_currentFrameIndex];
    }

    DeletionQueue& getCurrentDeletionQueue()
    {
        return getCurrentFrame().deletionQueue;
    }
    const DeletionQueue& getCurrentDeletionQueue() const
    {
        return getCurrentFrame().deletionQueue;
    }

    std::array<FrameData, FRAME_COUNT>& getFrames()
    {
        return m_frames;
    }
    const std::array<FrameData, FRAME_COUNT>& getFrames() const
    {
        return m_frames;
    }

    uint32_t getCurrentFrameIndex() const
    {
        return m_currentFrameIndex;
    }

    void advance();

private:
    std::array<FrameData, FRAME_COUNT> m_frames;
    uint32_t m_currentFrameIndex{0};

    VkDevice m_device;
};