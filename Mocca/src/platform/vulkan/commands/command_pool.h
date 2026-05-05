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

    CommandPool(CommandPool&&) noexcept;
    CommandPool& operator=(CommandPool&&) noexcept;

    const std::vector<VkCommandBuffer>& getBuffers() const
    {
        return m_buffers;
    }

    VkCommandBuffer getNextBuffer();

    void allocateBuffers(uint32_t count);
    void reset();

private:
    VkDevice m_logicalDevice{VK_NULL_HANDLE};
    VkCommandPool m_commandPool{VK_NULL_HANDLE};
    uint32_t m_usedCount = 0;
    std::vector<VkCommandBuffer> m_buffers;
};