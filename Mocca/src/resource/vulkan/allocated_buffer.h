#pragma once

#include <volk.h>
#include <vma/vk_mem_alloc.h>

class AllocatedBuffer
{
public:
    AllocatedBuffer() = default;

    AllocatedBuffer(VmaAllocator allocator, size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

    AllocatedBuffer(const AllocatedBuffer&) = delete;
    AllocatedBuffer& operator=(const AllocatedBuffer&) = delete;
    AllocatedBuffer(AllocatedBuffer&& other) noexcept;
    AllocatedBuffer& operator=(AllocatedBuffer&& other) noexcept;

    ~AllocatedBuffer();

    // testing out somethin new
    VkBuffer getBuffer() const
    {
        return m_buffer;
    }

    void* getMappedData() const
    {
        return m_info.pMappedData;
    }

private:
    VmaAllocator m_allocator{VK_NULL_HANDLE};
    VkBuffer m_buffer{VK_NULL_HANDLE};
    VmaAllocation m_allocation{VK_NULL_HANDLE};
    VmaAllocationInfo m_info{};
};