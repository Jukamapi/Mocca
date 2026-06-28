#pragma once
#include <vma/vk_mem_alloc.h>

// this class handles the creation of VmaAllocator and integrating volk into it
class GpuAlloc
{
public:
    GpuAlloc(VkPhysicalDevice physicalDevice, VkDevice device, VkInstance instance);
    ~GpuAlloc();

    GpuAlloc(const GpuAlloc&) = delete;
    GpuAlloc& operator=(const GpuAlloc&) = delete;
    GpuAlloc(GpuAlloc&&) = delete;
    GpuAlloc& operator=(GpuAlloc&&) = delete;

    const VmaAllocator& getVmaAllocator() const
    {
        return m_allocator;
    }

private:
    VmaAllocator m_allocator;
};