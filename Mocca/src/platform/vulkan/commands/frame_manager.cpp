#include "frame_manager.h"

#include "platform/vulkan/commands/command_pool.h"
#include "platform/vulkan/vk_check.h"


FrameManager::FrameManager(const QueueFamilyIndices& indices, VkDevice device)
    : m_device(device), m_frames{{FrameData(indices, device), FrameData(indices, device)}}
{
    VkSemaphoreCreateInfo semaphoreInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkFenceCreateInfo fenceInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    try
    {
        for(int i = 0; i < FRAME_COUNT; i++)
        {
            m_frames[i].commandPool.allocateBuffers(1);

            VK_CHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_frames[i].imageAvailableSemaphore));
            VK_CHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_frames[i].renderFinishedSemaphore));
            VK_CHECK(vkCreateFence(m_device, &fenceInfo, nullptr, &m_frames[i].renderFence));
        }
    }
    catch(...)
    {
        for(int i = 0; i < FRAME_COUNT; i++)
        {
            if(m_frames[i].imageAvailableSemaphore)
            {
                vkDestroySemaphore(m_device, m_frames[i].imageAvailableSemaphore, nullptr);
            }
            if(m_frames[i].renderFinishedSemaphore)
            {
                vkDestroySemaphore(m_device, m_frames[i].renderFinishedSemaphore, nullptr);
            }
            if(m_frames[i].renderFence)
            {
                vkDestroyFence(m_device, m_frames[i].renderFence, nullptr);
            }
        }

        throw;
    }
}

void FrameManager::advance()
{
    m_currentFrameIndex = (m_currentFrameIndex + 1) % FRAME_COUNT;
}

FrameManager::~FrameManager()
{
    for(int i = 0; i < FRAME_COUNT; i++)
    {
        m_frames[i].deletionQueue.flush();

        vkDestroySemaphore(m_device, m_frames[i].imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(m_device, m_frames[i].renderFinishedSemaphore, nullptr);
        vkDestroyFence(m_device, m_frames[i].renderFence, nullptr);
    }
}