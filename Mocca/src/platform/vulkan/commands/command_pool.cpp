#include "command_pool.h"

#include "core/types.h"
#include "platform/vulkan/vk_check.h"


CommandPool::CommandPool(const QueueFamilyIndices& indices, VkDevice device) : m_logicalDevice(device)
{
    VkCommandPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = 0,
        .queueFamilyIndex = indices.graphicsFamily.value(),
    };

    VK_CHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &m_commandPool));
}

CommandPool::CommandPool(CommandPool&& other) noexcept
{
    *this = std::move(other);
}

CommandPool& CommandPool::operator=(CommandPool&& other) noexcept
{
    if(this != &other)
    {
        if(m_commandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);
        }

        m_logicalDevice = other.m_logicalDevice;
        m_commandPool = other.m_commandPool;
        m_buffers = std::move(other.m_buffers);
        m_usedCount = other.m_usedCount;

        other.m_commandPool = VK_NULL_HANDLE;
        other.m_logicalDevice = VK_NULL_HANDLE;
        other.m_usedCount = 0;
    }
    return *this;
}

void CommandPool::allocateBuffers(uint32_t count)
{
    uint32_t oldSize = static_cast<uint32_t>(m_buffers.size());
    m_buffers.resize(oldSize + count);

    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = count,
    };

    VK_CHECK(vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &m_buffers[oldSize]));
}

VkCommandBuffer CommandPool::getNextBuffer()
{
    if(m_usedCount >= m_buffers.size())
    {
        allocateBuffers(1);
    }

    return m_buffers[m_usedCount++];
}

void CommandPool::reset()
{
    // we can do this as we set the bit in info
    m_usedCount = 0;
    vkResetCommandPool(m_logicalDevice, m_commandPool, 0);
}

CommandPool::~CommandPool()
{
    if(m_commandPool != VK_NULL_HANDLE)
    {
        // frees command buffers automatically
        vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);
    }
}