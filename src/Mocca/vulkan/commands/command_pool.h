#pragma once

#include <volk.h>

#include <vector>

class PhysicalDevice;
struct QueueFamilyIndices;

class CommandPool
{
public:
    CommandPool(const QueueFamilyIndices& indices, VkDevice device);
    ~CommandPool();

    CommandPool(const CommandPool&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;
    CommandPool(CommandPool&&) = delete;
    CommandPool& operator=(CommandPool&&) = delete;

    std::vector<VkCommandBuffer> getBuffers() const
    {
        return m_buffers;
    }

    void allocateBuffers(uint32_t count);
    void reset() const;

private:
    VkDevice m_logicalDevice{VK_NULL_HANDLE};
    VkCommandPool m_commandPool{VK_NULL_HANDLE};
    std::vector<VkCommandBuffer> m_buffers;
};