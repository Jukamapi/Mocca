#include "command_pool.h"
#include "Mocca/vulkan/vk_check.h"

#include "Mocca/core/types.h"

CommandPool::CommandPool(const QueueFamilyIndices& indices, VkDevice device) : m_logicalDevice(device)
{
    VkCommandPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = indices.graphicsFamily.value(),
    };

    VK_CHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &m_commandPool));
}

void CommandPool::allocateBuffers(uint32_t count)
{
    m_buffers.resize(m_buffers.size() + count);

    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = count,
    };

    VK_CHECK(vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, m_buffers.data()));
}

void CommandPool::reset() const
{
    // we can do this as we set the bit in info
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