#include "allocated_buffer.h"

#include "core/vk_check.h"


AllocatedBuffer::AllocatedBuffer(
    VmaAllocator allocator, size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage
)
    : m_allocator(allocator)
{
    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = allocSize,
        .usage = usage,
    };

    VmaAllocationCreateInfo vmaInfo{
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = memoryUsage,
    };

    VK_CHECK(vmaCreateBuffer(m_allocator, &bufferInfo, &vmaInfo, &m_buffer, &m_allocation, &m_info));
}

AllocatedBuffer::AllocatedBuffer(AllocatedBuffer&& other) noexcept
    : m_allocator(other.m_allocator),
      m_buffer(other.m_buffer),
      m_allocation(other.m_allocation),
      m_info(other.m_info)
{
    other.m_buffer = VK_NULL_HANDLE;
    other.m_allocation = VK_NULL_HANDLE;
}

AllocatedBuffer& AllocatedBuffer::operator=(AllocatedBuffer&& other) noexcept
{
    if(this != &other)
    {
        if(m_buffer != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
        }

        m_allocator = other.m_allocator;
        m_buffer = other.m_buffer;
        m_allocation = other.m_allocation;
        m_info = other.m_info;

        other.m_buffer = VK_NULL_HANDLE;
        other.m_allocation = VK_NULL_HANDLE;
    }

    return *this;
}

AllocatedBuffer::~AllocatedBuffer()
{
    vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
}
